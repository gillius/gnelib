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
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/ConnectionEventGenerator.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/SocketPair.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/GNE.h"
#include "../include/gnelib/EventThread.h"

namespace GNE {

//##ModelId=3B0753810073
Connection::Connection(int outRate, int inRate, ConnectionListener* listener)
: connecting(false), connected(false), rlistener(NULL), ulistener(NULL) {
  ps = new PacketStream(outRate, inRate, *this);
  eventListener = new EventThread(listener);
}

//##ModelId=3B0753810076
Connection::~Connection() {
  disconnect();
  delete ps;
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
}

//##ModelId=3BCE75A80280
ConnectionListener* Connection::getListener() const {
  return eventListener->getListener();
}

//##ModelId=3BCE75A80282
void Connection::setListener(ConnectionListener* listener) {
  eventListener->setListener(listener);
}

//##ModelId=3B0753810078
PacketStream& Connection::stream() {
  return *ps;
}

//##ModelId=3B0753810079
ConnectionStats Connection::getStats(int reliable) const {
  return sockets.getStats(reliable);
}

//##ModelId=3B075381007B
Address Connection::getLocalAddress(bool reliable) const {
  return Address(sockets.getLocalAddress(reliable));
}

//##ModelId=3B075381007E
Address Connection::getRemoteAddress(bool reliable) const {
  return Address(sockets.getRemoteAddress(reliable));
}

//##ModelId=3B0753810081
bool Connection::isConnected() const {
  return connected;
}

//##ModelId=3B0753810083
void Connection::disconnect() {
  sync.acquire();
  if (connecting || connected) {
    //This is necessary because we can't join on ps if it has already been
    //  shutdown and/or never started
    unreg(true, true);
    gnedbgo2(2, "disconnecting r: %i, u: %i", sockets.r, sockets.u);
    ps->shutDown();
    ps->join();
    onDisconnect();
    //This will also shutdown the EventThread, and we will join on it in the
    //destructor (because this could be our EventThread, and you can't join
    //on yourself).
    sockets.disconnect();
    connected = connecting = false;
  }
  sync.release();
}

//##ModelId=3B0753810084
void Connection::disconnectSendAll() {
  if (connected) {
    ps->waitToSendAll();
    disconnect();
  }
}

//##ModelId=3BB4208C0104
void Connection::onDisconnect() {
  eventListener->onDisconnect();
}

//##ModelId=3B0753810085
void Connection::onFailure(const Error& error) {
  eventListener->onFailure(error);
}

//##ModelId=3BB4208C01E0
void Connection::onError(const Error& error) {
  eventListener->onError(error);
}

//##ModelId=3B07538100AC
void Connection::onReceive() {
  eventListener->onReceive();
}

//##ModelId=3B07538100AE
void Connection::onDoneWriting() {
  eventListener->onDoneWriting();
}

void Connection::finishedConnecting() {
  sync.acquire();
  assert(connecting && !connected);
  connected = true;
  connecting = false;
  sync.release();
}

//##ModelId=3B07538100B0
void Connection::onReceive(bool reliable) {
  //Create buffer into a RawPacket
  NLbyte* buf = new NLbyte[RawPacket::RAW_PACKET_LEN];
  int temp = sockets.rawRead(reliable, buf, RawPacket::RAW_PACKET_LEN);
  if (temp == NL_INVALID) {
    NLenum error = nlGetError();
    if (error == NL_MESSAGE_END) {
      //in HawkNL 1.4b4 and later, this means that the connection was
      //closed on the network-level because the client disconnected or
      //has dropped.  Since we didn't get an "exit" packet, it's an error.
      processError(Error::ConnectionDropped);
    } else {
      //This is some other bad error that we need to report
      processError(Error::createLowLevelError(Error::Read));
    }
  } else if (temp == 0) {
    //In HawkNL 1.4b3 and earlier, this _USED_ to mean that...
    //This means the connection was closed on the network-level because
    //remote computer has purposely closed or has dropped.
    processError(Error::ConnectionDropped);
  } else {
    RawPacket raw(buf);
		
		//parse the packets and add them to the PacketStream
    bool errorCheck;
    Packet* next = NULL;
    while ((next = PacketParser::parseNextPacket(errorCheck, raw)) != NULL) {
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

//##ModelId=3BB4208C0280
void Connection::processError(const Error& error) {
  switch(error.getCode()) {
  case Error::UnknownPacket:
    errorSync.acquire();
    onError(error);
    errorSync.release();
    break;
  default:
    errorSync.acquire();
    onFailure(error);
    errorSync.release();
    disconnect();
    break;
  }
}

//##ModelId=3B075381004E
Connection::Listener::Listener(Connection& listener, bool isReliable) 
: conn(listener), reliable(isReliable) {
}

//##ModelId=3B0753810051
Connection::Listener::~Listener() {
}

//##ModelId=3B0753810053
void Connection::Listener::onReceive() {
  conn.onReceive(reliable);
}

//##ModelId=3B6E14AC0104
void Connection::reg(bool reliable, bool unreliable) {
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
}

//##ModelId=3B6E14AC01D6
void Connection::unreg(bool reliable, bool unreliable) {
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
}

} //Namespace GNE
