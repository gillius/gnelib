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

Connection::Connection(ConnectionListener* listener)
: ps(NULL), connecting(false), connected(false), rlistener(NULL),
ulistener(NULL), exiting(false) {
  eventListener = new EventThread(listener, this);
}

Connection::~Connection() {
  disconnect();
  if (eventListener->hasStarted()) {
    //disconnect calls onDisconnect, which should shut down the EventThread,
    //and allow detach and join to work as we expect.
    if (Thread::currentThread() == eventListener) {
      //This section of code allows the ConnectionListeners to delete their
      //associated Connections.
      eventListener->detach(true);
    } else {
      eventListener->join();
      delete eventListener;
    }
  } else {
    //If it was never started, we just delete it.
    delete eventListener;
  }
  delete ps;
}

ConnectionListener* Connection::getListener() const {
  return eventListener->getListener();
}

void Connection::setListener(ConnectionListener* listener) {
  eventListener->setListener(listener);
}

int Connection::getTimeout() {
  return eventListener->getTimeout();
}

void Connection::setTimeout(int ms) {
  eventListener->setTimeout(ms);
}

PacketStream& Connection::stream() {
  assert(ps != NULL);
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
  sync.acquire();
  if (connecting || connected) {
    //This is necessary because we can't join on ps if it has already been
    //  shutdown and/or never started
    unreg(true, true);
    gnedbgo2(2, "disconnecting r: %i, u: %i", sockets.r, sockets.u);
    ps->shutDown(); //PacketStream will try to send the required ExitPacket.
    ps->join();
    //Once we call onDisconnect we can be deleted at anytime.  But it is OK
    //to call this here because sync must be acquired and released until the
    //destructor completes therefore this function will complete before we
    //can be possibly deleted.
    eventListener->onDisconnect();
    //This will also shutdown the EventThread, and we will join on it in the
    //destructor (because this could be our EventThread, and you can't join
    //on yourself).
    connected = connecting = false;
  }
  //We always call the low-level disconnect in case errors happened while
  //opening before connecting or for some other reason they are left open --
  //and calling disconnect multiple times does not hurt.
  sockets.disconnect();
  sync.release();
}

void Connection::disconnectSendAll(int waitTime) {
  if (isConnected())
    ps->waitToSendAll(waitTime);
  disconnect();
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
  eventListener->onReceive();
}

void Connection::finishedConnecting() {
  sync.acquire();
  if (connecting && !connected) {
    //If this was not true, then we were disconnected before this (and before
    //the connecting code knows about it).
    connected = true;
    connecting = false;
  }
  sync.release();
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
        eventListener->onExit();
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
    if (!exiting)
      eventListener->onError(error);
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
    if (!exiting)
      eventListener->onFailure(error);
    break;
  }
}

Connection::Listener::Listener(Connection& listener, bool isReliable) 
: conn(listener), reliable(isReliable) {
}

Connection::Listener::~Listener() {
}

void Connection::Listener::onReceive() {
  conn.onReceive(reliable);
}

void Connection::reg(bool reliable, bool unreliable) {
  regSync.acquire();
  if (reliable && rlistener == NULL) {
    assert(sockets.r != NL_INVALID);
    rlistener = new Listener(*this, true);
    eGen->reg(sockets.r, rlistener);
    gnedbgo1(3, "Registered reliable socket %i", sockets.r);
  }
  if (unreliable && ulistener == NULL) {
    assert(sockets.u != NL_INVALID);
    ulistener = new Listener(*this, false);
    eGen->reg(sockets.u, ulistener);
    gnedbgo1(3, "Registered unreliable socket %i", sockets.u);
  }
  regSync.release();
}

void Connection::unreg(bool reliable, bool unreliable) {
  regSync.acquire();
  if (reliable && rlistener != NULL) {
    eGen->unreg(sockets.r);
    delete rlistener;
    rlistener = NULL;
    gnedbgo1(3, "Unregistered reliable socket %i", sockets.r);
  }
  if (unreliable && ulistener != NULL) {
    eGen->unreg(sockets.u);
    delete ulistener;
    ulistener = NULL;
    gnedbgo1(3, "Unregistered unreliable socket %i", sockets.u);
  }
  regSync.release();
}

} //Namespace GNE
