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
#include "../include/gnelib/ClientConnection.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/SyncConnection.h"

namespace GNE {

//##ModelId=3B075380037F
ClientConnection::ClientConnection(int outRate, int inRate, ConnectionListener* listener)
  : Connection(outRate, inRate, listener) {
  gnedbgo(5, "created");
}

  //##ModelId=3B07538003B8
ClientConnection::~ClientConnection() {
  gnedbgo(5, "destroyed");
}

/**
 * \todo time sync with server, and do negotiations.
 */
//##ModelId=3B07538003BA
void ClientConnection::run() {
  gnedbgo1(1, "Trying to connect to %s", address.toString().c_str());
  NLaddress temp = address.getAddress();
  NLboolean check = nlConnect(sockets.r, &temp);
  if (check == NL_TRUE) {
    gnedbgo2(2, "connection r: %i, u: %i", sockets.r, sockets.u);
    assert(eventListener != NULL);
    try {
      SyncConnection sync(this);
      reg(true, false);
      ps->start();
      connecting = true;
      eventListener->onConnect(sync); //SyncConnection will relay this
      sync.release();
      connected = true;
      connecting = false;
    } catch (Error e) {
      //The client should catch any errors in onConnect, but release can
      //give an error, so we catch it.
      gnedbgo1(1, "Connection failure: %s", e.toString().c_str());
      eventListener->onConnectFailure(e);
    }
  } else {
    assert(eventListener != NULL);
    Error err = Error::createLowLevelError();
    err.setCode(Error::ConnectionTimeOut);
    gnedbgo1(1, "Connection failure: %s", err.toString().c_str());
    eventListener->onConnectFailure(err);
  }
}

//##ModelId=3B07538003BB
bool ClientConnection::open(const Address& dest, int localPort) {
  if (!dest)
    return true;
  else {
    address = dest;
    sockets.r = nlOpen(localPort, NL_RELIABLE_PACKETS);
    return (sockets.r == NL_INVALID);
  }
}

//##ModelId=3B07538003C1
void ClientConnection::connect() {
  assert(sockets.r != NL_INVALID);
  assert(address);
  start();
}

}






