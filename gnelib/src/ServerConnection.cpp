/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@webzone.net)
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
#include "ServerConnection.h"
#include "Connection.h"

//##ModelId=3AE59FAF033E
ServerConnection::ServerConnection(int outRate, int inRate) {
}

//##ModelId=3AE59FAF037A
ServerConnection::~ServerConnection() {
  nlClose(socket);
}

//##ModelId=3B009F57037A
bool ServerConnection::open(int port) {
  socket = nlOpen(port, NL_RELIABLE_PACKETS);
  return (socket == NL_INVALID);
}

//##ModelId=3AE5A0F60028
bool ServerConnection::listen() {
  //return (nlListen(socket) == NL_FALSE);
  assert(nlListen(socket) == NL_TRUE);
  NLsocket sock = nlAcceptConnection(socket);
  assert(sock != NL_INVALID);
  nlClose(sock);
  return false;
}

//##ModelId=3AE5A1310208
void ServerConnection::onListenFailure(Connection::FailureType errorType) {
}

//##ModelId=3AE5AF95021C
void ServerConnection::onReceive() {
}

//##ModelId=3B00A3BE0352
NLaddress ServerConnection::getLocalAddress() const {
  NLaddress ret;
  assert(socket != NL_INVALID);
  nlGetLocalAddr(socket, &ret);
  return ret;
}

//##ModelId=3B009DCD03C0
ServerConnection::ServerListener::ServerListener(ServerConnection& listener)
: conn(listener) {
}

//##ModelId=3B009DCE0046
ServerConnection::ServerListener::~ServerListener() {
}

//##ModelId=3B009BAA00F0
void ServerConnection::ServerListener::onReceive() {
}

