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
#include "../include/gnelib/GNE.h"
#include "../include/gnelib/Address.h"

namespace GNE {

//##ModelId=3B07538102E4
ServerConnectionListener::ServerConnectionListener()
: listening(false), listener(NULL) {
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

//##ModelId=3B075381030F
void ServerConnectionListener::onReceive() {
  NLsocket sock = nlAcceptConnection(socket);
  if (sock != NL_INVALID) {
		int inRate, outRate;
		ConnectionListener* listener;
		getNewConnectionParams(inRate, outRate, listener);

		ServerConnection* newConn = new ServerConnection(outRate, inRate, listener, sock, this);
		gnedbgo2(4, "Spawning a new ServerConnection %x on socket %i", newConn, sock);
		newConn->start();
	} else {
		Error err = Error::createLowLevelError();
		gnedbgo1(1, "Listening failure (accept failed): %s", err.toString().c_str());
		onListenFailure(err, NULL, NULL);
	}
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





