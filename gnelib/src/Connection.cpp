/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@mail.rit.edu)
 * Project website: http://www.rit.edu/~jpw9607/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/ConnectionStats.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/ExitPacket.h"
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/ConnectionEventGenerator.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Errors.h"
#include "../include/gnelib/SocketPair.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/GNE.h"
#include "../include/gnelib/EventThread.h"

namespace GNE {

Connection::Connection()
: connecting(false), connected(false), exiting(false) {
}

Connection::~Connection() {
  //no need to disconnect here because we should never have been started or
  //have been disconnected.
  assert( !connecting && !connected );
}

ConnectionListener* Connection::getListener() const {
  EventThread::sptr et = eventThread.lock();
  if ( et )
    return et->getListener();
  else
    return NULL;
}

void Connection::setListener(ConnectionListener* listener) {
  EventThread::sptr et = eventThread.lock();
  if ( et )
    et->setListener( listener );
}

int Connection::getTimeout() {
  EventThread::sptr et = eventThread.lock();
  if ( et )
    return et->getTimeout();
  else
    return 0;
}

void Connection::setTimeout(int ms) {
  EventThread::sptr et = eventThread.lock();
  if ( et )
    et->setTimeout(ms);
}

PacketStream& Connection::stream() {
  assert(ps);
  return *ps;
}

ConnectionStats Connection::getStats(int reliable) const {
  return sockets.getStats(reliable);
}

Address Connection::getLocalAddress(bool reliable) const {
  return Address(sockets.getLocalAddress(reliable));
}

Address Connection::getRemoteAddress(bool reliable) const {
  return Address(sockets.getRemoteAddress(reliable));
}

bool Connection::isConnected() const {
  return connected;
}

void Connection::disconnect() {
  LockMutex lock( sync );
  if (connecting || connected) {
    //This is necessary because we can't join on ps if it has already been
    //  shutdown and/or never started
    unreg(true, true);
    gnedbgo2(2, "disconnecting r: %i, u: %i", sockets.r, sockets.u);
    ps->shutDown(); //PacketStream will try to send the required ExitPacket.
    ps->join();

    //This will also shutdown the EventThread, and we will join on it in the
    //destructor (because this could be our EventThread, and you can't join
    //on yourself).
    EventThread::sptr et = eventThread.lock();
    assert( et ); //if we are connected we HAVE to have a valid EventThread
    et->onDisconnect();

    connected = connecting = false;
  }
  //We always call the low-level disconnect in case errors happened while
  //opening before connecting or for some other reason they are left open --
  //and calling disconnect multiple times does not hurt.
  sockets.disconnect();
}

void Connection::disconnectSendAll(int waitTime) {
  LockMutex lock( sync );
  if (isConnected())
    ps->waitToSendAll(waitTime);
  disconnect();
}

void Connection::setThisPointer( const wptr& weakThis ) {
  this_ = weakThis;
  eventThreadTemp = EventThread::create( NULL, weakThis.lock() );
  eventThread = eventThreadTemp;
}

void Connection::startEventThread() {
  eventThreadTemp->start();
  eventThreadTemp.reset();
}

void Connection::addHeader(RawPacket& raw) {
  raw << (gbyte)'G' << (gbyte)'N' << (gbyte)'E';
}

void Connection::addVersions(RawPacket& raw) {
  GNEProtocolVersionNumber us = GNE::getGNEProtocolVersion();
  //Write the GNE version numbers.
  raw << us.version << us.subVersion << us.build;

  //Write the whole game name buffer
  raw.writeRaw((const gbyte*)GNE::getGameName(), GNE::MAX_GAME_NAME_LEN + 1);

  //Write the user version
  raw << GNE::getUserVersion();
}

void Connection::checkHeader(RawPacket& raw,
                             ProtocolViolation::ViolationType t) {
  gbyte headerG, headerN, headerE;
  raw >> headerG >> headerN >> headerE;

  if (headerG != 'G' || headerN != 'N' || headerE != 'E')
    throw ProtocolViolation(t);
}

void Connection::checkVersions(RawPacket& raw) {
  //Get the version numbers
  GNEProtocolVersionNumber them;
  raw >> them.version >> them.subVersion >> them.build;

  //Read the game name
  gbyte rawName[GNE::MAX_GAME_NAME_LEN + 1];
  raw.readRaw(rawName, GNE::MAX_GAME_NAME_LEN + 1);
  //And convert it to a string, making sure it is of the proper length and
  //NULL-terminated.
  char gameName[GNE::MAX_GAME_NAME_LEN + 1];
  strncpy(gameName, (const char*)rawName, GNE::MAX_GAME_NAME_LEN);

  //Read the user version number
  guint32 themUser;
  raw >> themUser;

  //This will throw an Error of the versions are wrong.
  GNE::checkVersions(them, gameName, themUser);
}

void Connection::onReceive() {
  EventThread::sptr et = eventThread.lock();
  assert( et );
  et->onReceive();
}

void Connection::finishedConnecting() {
  LockMutex lock( sync );

  if (connecting && !connected) {
    //If this was not true, then we were disconnected before this (and before
    //the connecting code knows about it).
    connected = true;
    connecting = false;
  }
}

void Connection::onReceive(bool reliable) {
  //Create buffer into a RawPacket
  gbyte* buf = new gbyte[RawPacket::RAW_PACKET_LEN];
  int temp = sockets.rawRead(reliable, buf, RawPacket::RAW_PACKET_LEN);
  if (temp == NL_INVALID) {
    NLenum error = nlGetError();
    if (error == NL_MESSAGE_END) {
      //in HawkNL 1.4b4 and later, this means that the connection was
      //closed on the network-level because the client disconnected or
      //has dropped.  If we got an ExitPacket, this will be ignored.
      processError(Error::ConnectionDropped);
    } else {
      //This is some other bad error that we need to report
      processError(LowLevelError(Error::Read));
    }
  } else if (temp == 0) {
    //In HawkNL 1.4b3 and earlier, this _USED_ to mean that...
    //This means the connection was closed on the network-level because
    //remote computer has purposely closed or has dropped.
    //We should never get this now, because HawkNL traps this message, but
    //for completeness we check for it anyways.
    processError(Error::ConnectionDropped);
  } else {
    RawPacket raw(buf);
    
    //parse the packets and add them to the PacketStream
    bool errorCheck;
    Packet* next = NULL;
    while ((next = PacketParser::parseNextPacket(errorCheck, raw)) != NULL) {
      //We want to intercept ExitPackets, else we just add it.
      if (next->getType() == ExitPacket::ID) {
        exiting = true;
        EventThread::sptr et = eventThread.lock();
        assert( et );
        et->onExit();
      } else
        ps->addIncomingPacket(next);
    }
    delete[] buf;
    
    //Start the event
    if (errorCheck == false) {
      //These are level 4 since the explicit event log is generated in onFailure
      gnedbgo1(4, "Unknown packet encountered in a message that has %i bytes", temp);
      processError(Error::UnknownPacket);
    } else {
      onReceive();
    }
  }
}

/**
 * \bug we can still get multiple failures if the read fails then PacketStream
 *      fails.  I don't think this will cause a deadlock though if we unreg,
 *      trigger onFailure then PacketStream fails because then the eGen won't
 *      block.
 */
void Connection::processError(const Error& error) {
  //If we got an ExitPacket, then any errors that we get should be ignored
  //because the socket will fail when we disconnect.
  switch(error.getCode()) {
  case Error::UnknownPacket:
    if (!exiting) {
      EventThread::sptr et = eventThread.lock();
      assert( et );
      et->onError(error);
    }
    break;
  default:
    //The EventThread will call disconnect.  This will prevent a deadlock
    //where PacketStream and another thread is trying to disconnect at the
    //same time.
    //But if we don't disconnect we generate endless error messages because
    //we need to unregister immediately.  So we do that.
    unreg(true, true);
    //We call onFailure after unreg because a deadlock will occur if the
    //EventThread tries to disconnect before we unreg.
    if (!exiting) {
      EventThread::sptr et = eventThread.lock();
      assert( et );
      et->onFailure(error);
    }
    break;
  }
}

Connection::Listener::Listener(const Connection::sptr& listener, bool isReliable) 
: conn(listener), reliable(isReliable) {
}

Connection::Listener::~Listener() {
}

void Connection::Listener::onReceive() {
  conn->onReceive(reliable);
}

void Connection::reg(bool reliable, bool unreliable) {
  LockMutex lock( regSync );

  if (reliable && rlistener.expired()) {
    assert(sockets.r != NL_INVALID);
    Listener::sptr temp = Listener::sptr( new Listener( this_.lock(), true ) );
    rlistener = temp;
    eGen->reg( sockets.r, temp );
    gnedbgo1(3, "Registered reliable socket %i", sockets.r);
  }
  if (unreliable && rlistener.expired()) {
    assert(sockets.u != NL_INVALID);
    Listener::sptr temp = Listener::sptr( new Listener( this_.lock(), false ) );
    ulistener = temp;
    eGen->reg( sockets.u, temp );
    gnedbgo1(3, "Registered unreliable socket %i", sockets.u);
  }
}

void Connection::unreg(bool reliable, bool unreliable) {
  LockMutex lock( regSync );

  if (reliable && rlistener.lock()) {
    eGen->unreg(sockets.r);
    gnedbgo1(3, "Unregistered reliable socket %i", sockets.r);
  }
  if (unreliable && ulistener.lock()) {
    eGen->unreg(sockets.u);
    gnedbgo1(3, "Unregistered unreliable socket %i", sockets.u);
  }
}

} //Namespace GNE
