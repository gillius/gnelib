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
#include "ClientConnection.h"

//##ModelId=3AE59FAB0000
ClientConnection::ClientConnection(int outRate, int inRate)
: Connection(outRate, inRate) {
}

//##ModelId=3AE59FAB003C
ClientConnection::~ClientConnection() {
}

/**
 * \todo time sync with server
 */
//##ModelId=3AE5AF7A0384
void ClientConnection::run() {
  NLboolean check = nlConnect(rsocket, &address);
  if (check == NL_TRUE) {
    connected = true;
    onConnect();
  } else {
    onConnectFailure(ConnectionTimeOut);
  }
}

//##ModelId=3B00C2E40302
bool ClientConnection::open(std::string dest, int port) {
  NLaddress addr;
  nlStringToAddr((NLbyte*)dest.c_str(), &addr);
  return open(addr, port);
}

//##ModelId=3B00C2E50104
bool ClientConnection::open(NLaddress dest, int port) {
  address = dest;
  rsocket = nlOpen(port, NL_RELIABLE);
  return (rsocket == NL_INVALID);
}

//##ModelId=3AE59FD4019A
void ClientConnection::connect() {
  start();
}