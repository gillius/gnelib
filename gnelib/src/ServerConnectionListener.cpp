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
#include "../include/gnelib/ServerConnectionListener.h"
#include "../include/gnelib/ConnectionEventGenerator.h"
#include "../include/gnelib/ServerConnection.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/ConnectionParams.h"
#include "../include/gnelib/GNE.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/Errors.h"

namespace GNE {

ServerConnectionListener::ServerConnectionListener()
: listening(false) {
  gnedbgo(5, "created");
}

ServerConnectionListener::~ServerConnectionListener() {
  gnedbgo(5, "destroyed");
}

bool ServerConnectionListener::open(int port) {
  socket = nlOpen(port, NL_RELIABLE_PACKETS);
  return (socket == NL_INVALID);
}

void ServerConnectionListener::close() {
  LockMutex lock(sync);

  if (listening) {
    gnedbgo1(3, "Unregistering listen socket %i", socket);
    GNE::eGen->unreg(socket);
    nlClose(socket);
    listening = false;
  }
}

bool ServerConnectionListener::listen() {
  LockMutex lock(sync);

  NLboolean ret = nlListen(socket);
  if (ret == NL_TRUE) {
    gnedbgo1(3, "Registering listen socket %i", socket);
    GNE::eGen->reg(socket, ServerListener::sptr( new ServerListener( this_.lock() ) ) );
    listening = true;
    return false;
  }
  return true;
}

bool ServerConnectionListener::isListening() const {
  return listening;
}

void ServerConnectionListener::onReceive() {
  LockMutex lock(sync);

  NLsocket sock = nlAcceptConnection(socket);
  if (sock != NL_INVALID) {
    ConnectionParams params;
    getNewConnectionParams(params);

    if (!params) {
      //If the params were valid
      ServerConnection::sptr newConn = ServerConnection::create(params, sock, this);
      gnedbgo2(4, "Spawning a new ServerConnection %x on socket %i",
        newConn, sock);
      newConn->start();
    } else {
      //If the params are not valid, report the error
      onListenFailure(Error(Error::OtherGNELevelError), Address(),
        params.getListener());
    }
  } else {
    LowLevelError err = LowLevelError();
    gnedbgo1(1, "Listening failure (accept failed): %s",
      err.toString().c_str());
    onListenFailure(err, Address(), NULL);
  }
}

Address ServerConnectionListener::getLocalAddress() const {
  LockMutex lock(sync);

  if (socket != NL_INVALID) {
    NLaddress ret;
    nlGetLocalAddr(socket, &ret);
    return Address(ret);
  } else {
    return Address();
  }
}

void ServerConnectionListener::processOnListenFailure( const Error& error, const Address& from, ConnectionListener* listener) {
  LockMutex lock( sync );
  onListenFailure( error, from, listener );
}

void ServerConnectionListener::processOnListenSuccess( ConnectionListener* listener ) {
  LockMutex lock( sync );
  onListenSuccess( listener );
}

void ServerConnectionListener::onListenSuccess(ConnectionListener* listener) {
  //The default behavior for this event is to do nothing.
}

ServerConnectionListener::ServerListener::ServerListener(const ServerConnectionListener::sptr& listener)
: conn(listener) {
}

ServerConnectionListener::ServerListener::~ServerListener() {
}

void ServerConnectionListener::ServerListener::onReceive() {
  conn->onReceive();
}

}





