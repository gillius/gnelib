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

#include "gneintern.h"
#include "ServerConnectionListener.h"
#include "ServerConnectionCreator.h"
#include "ConnectionEventGenerator.h"
#include "ServerConnection.h"
#include "Connection.h"
#include "GNE.h"
#include "Address.h"

namespace GNE {

//##ModelId=3B07538102E4
ServerConnectionListener::ServerConnectionListener(int outRate2, int inRate2, ServerConnectionCreator* creator2)
: listening(false), listener(NULL), inRate(inRate2), outRate(outRate2), creator(creator2) {
  listener = new ServerListener(*this);
}

//##ModelId=3B0753810303
ServerConnectionListener::~ServerConnectionListener() {
  if (listening) {
		gnedbgo1(3, "Unregistering listen socket %i", socket);
    GNE::eGen->unreg(socket);
    nlClose(socket);
  }
  delete listener;
  delete creator;
}

//##ModelId=3B0753810305
bool ServerConnectionListener::open(int port) {
  socket = nlOpen(port, NL_RELIABLE_PACKETS);
  return (socket == NL_INVALID);
}

//##ModelId=3B0753810307
bool ServerConnectionListener::listen() {
  NLboolean ret = nlListen(socket);
  if (ret == NL_TRUE) {
		gnedbgo1(3, "Registering listen socket %i", socket);
    GNE::eGen->reg(socket, listener);
    listening = true;
    return false;
  }
  return true;
}

//##ModelId=3B0753810308
bool ServerConnectionListener::isListening() const {
  return listening;
}

//##ModelId=3B075381030A
void ServerConnectionListener::onListenFailure(Error error) {
}

//##ModelId=3B075381030F
void ServerConnectionListener::onReceive() {
  NLsocket sock = nlAcceptConnection(socket);
  assert(sock != NL_INVALID);
  ServerConnection* newConn = creator->create(outRate, inRate, sock);
	gnedbgo2(4, "Spawning a new ServerConnection %x on socket %i", newConn, sock);
  newConn->start();
}

//##ModelId=3B075381030D
Address ServerConnectionListener::getLocalAddress() const {
  assert(socket != NL_INVALID);
  NLaddress ret;
  nlGetLocalAddr(socket, &ret);
  return Address(ret);
}

//##ModelId=3B07538102D0
ServerConnectionListener::ServerListener::ServerListener(ServerConnectionListener& listener)
: conn(listener) {
}

//##ModelId=3B07538102D2
ServerConnectionListener::ServerListener::~ServerListener() {
}

//##ModelId=3B07538102D4
void ServerConnectionListener::ServerListener::onReceive() {
  conn.onReceive();
}

}


