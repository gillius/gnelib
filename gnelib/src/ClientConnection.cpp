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
#include "../include/gnelib/EventThread.h"

namespace GNE {

//##ModelId=3B075380037F
ClientConnection::ClientConnection(int outRate, int inRate, ConnectionListener* listener)
  : Connection(outRate, inRate, listener), Thread("CliConn") {
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
    //We don't want to doubly-wrap SyncConnections, so we check for a wrapped
    //one here and else make our own.
    bool ourSConn = false;
    if (!sConnPtr) {
      sConnPtr = new SyncConnection(this);
      ourSConn = true;
    } else
      assert(sConnPtr == getListener());
    //The sConn reference variable is used only for syntactical convienence.
    SyncConnection& sConn = *sConnPtr;
    
    bool onConnectFinished = false;
    assert(eventListener != NULL);
    try {
      //We only want to hold events on our own SyncConnection.  On a user
      //supplied SyncConnection, when it fails we will fail, and
      //SyncConnection::connect() will throw an error.
      if (ourSConn)
        sConn.startConnect();
      ps->start();
      connecting = true;
      eventListener->start();
      reg(true, false);

      //Do GNE protocol negotiations here

      gnedbgo2(2, "Starting onConnect r: %i, u: %i", sockets.r, sockets.u);
      getListener()->onConnect(sConn); //SyncConnection will relay this
      onConnectFinished = true;
      finishedConnecting();

      if (ourSConn) {
        sConn.endConnect(true);
        sConn.release();
        delete sConnPtr;
      }
    } catch (Error e) {
      if (!onConnectFinished) {
        if (ourSConn)
          sConn.endConnect(false);
        getListener()->onConnectFailure(e);
      }
      //else onDisconnect should get called.
      if (ourSConn)
        delete sConnPtr;
    }
  } else {
    assert(eventListener != NULL);
    Error err = Error::createLowLevelError(Error::ConnectionTimeOut);
    gnedbgo1(1, "Connection failure: %s", err.toString().c_str());
    getListener()->onConnectFailure(err);
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
void ClientConnection::connect(SyncConnection* wrapped) {
  assert(sockets.r != NL_INVALID);
  assert(address);
  sConnPtr = wrapped;
  start();
}

}






