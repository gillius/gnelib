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
#include "ClientConnection.h"

namespace GNE {

//##ModelId=3B075380037F
ClientConnection::ClientConnection(int outRate, int inRate)
: Connection(outRate, inRate) {
}

//##ModelId=3B07538003B8
ClientConnection::~ClientConnection() {
}

/**
 * \todo time sync with server
 */
//##ModelId=3B07538003BA
void ClientConnection::run() {
  NLboolean check = nlConnect(rsocket, &address);
  if (check == NL_TRUE) {
    connected = true;
    onConnect();
  } else {
    onConnectFailure(ConnectionTimeOut);
  }
}

//##ModelId=3B07538003BB
bool ClientConnection::open(std::string dest, int port) {
  NLaddress addr;
  nlStringToAddr((NLbyte*)dest.c_str(), &addr);
  return open(addr, port);
}

//##ModelId=3B07538003BE
bool ClientConnection::open(NLaddress dest, int port) {
  address = dest;
  rsocket = nlOpen(port, NL_RELIABLE);
  return (rsocket == NL_INVALID);
}

//##ModelId=3B07538003C1
void ClientConnection::connect() {
  start();
}

}


