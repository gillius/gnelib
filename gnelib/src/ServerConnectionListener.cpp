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
#include "ServerConnectionListener.h"
#include "ServerConnectionCreator.h"
#include "ConnectionEventGenerator.h"
#include "ServerConnection.h"
#include "Connection.h"
#include "GNE.h"

//##ModelId=3B01D3D70208
ServerConnectionListener::ServerConnectionListener(int outRate2, int inRate2, ServerConnectionCreator* creator2)
: listening(false), listener(NULL), inRate(inRate2), outRate(outRate2), creator(creator2) {
  listener = new ServerListener(*this);
}

//##ModelId=3B01D3D7020B
ServerConnectionListener::~ServerConnectionListener() {
  if (listening) {
    GNE::eGen->unreg(socket);
    nlClose(socket);
  }
  delete listener;
  delete creator;
}

//##ModelId=3B01D3D70227
bool ServerConnectionListener::open(int port) {
  socket = nlOpen(port, NL_RELIABLE_PACKETS);
  return (socket == NL_INVALID);
}

//##ModelId=3B01D3D70229
bool ServerConnectionListener::listen() {
  NLboolean ret = nlListen(socket);
  if (ret == NL_TRUE) {
    GNE::eGen->reg(socket, listener);
    listening = true;
    return false;
  }
  return true;
}

//##ModelId=3B0334A90230
bool ServerConnectionListener::isListening() const {
  return listening;
}

//##ModelId=3B0334A90348
void ServerConnectionListener::onListenFailure(Connection::FailureType errorType) {
}

//##ModelId=3B01D3D70232
void ServerConnectionListener::onReceive() {
  NLsocket sock = nlAcceptConnection(socket);
  assert(sock != NL_INVALID);
  ServerConnection* newConn = creator->create(outRate, inRate, sock);
  newConn->start();
}

//##ModelId=3B01D3D70230
NLaddress ServerConnectionListener::getLocalAddress() const {
  NLaddress ret;
  assert(socket != NL_INVALID);
  nlGetLocalAddr(socket, &ret);
  return ret;
}

//##ModelId=3B01D3D701FA
ServerConnectionListener::ServerListener::ServerListener(ServerConnectionListener& listener)
: conn(listener) {
}

//##ModelId=3B01D3D701FC
ServerConnectionListener::ServerListener::~ServerListener() {
}

//##ModelId=3B01D3D701FE
void ServerConnectionListener::ServerListener::onReceive() {
  conn.onReceive();
}
