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
ClientConnection::ClientConnection(int outRate, int inRate, std::string address) 
: Connection(outRate, inRate, address) {
}

//##ModelId=3AE59FAB003C
ClientConnection::~ClientConnection() {
}

//##ModelId=3AE5AF7A0384
void ClientConnection::run() {
}

//##ModelId=3AE59FD4019A
void ClientConnection::connect() {
}

//##ModelId=3AE59FBB01A4
void ClientConnection::onConnect() {
}

//##ModelId=3AE59FCA0168
void ClientConnection::onConnectFailure(FailureType errorType) {
}


