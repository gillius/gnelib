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
#include "ServerConnection.h"

namespace GNE {

//##ModelId=3B075381027A
ServerConnection::ServerConnection(int outRate, int inRate, NLsocket rsocket2)
: Connection(outRate, inRate) {
  rsocket = rsocket2;
}

//##ModelId=3B075381027E
ServerConnection::~ServerConnection() {
}

/**
 * \todo implement negotiation
 * \bug this function relies on the fact that mutexes are recursive, which is
 *      not true over all pthreads implementations.  When
 *      ConnectionEventGenerator lauches each onReceive event in a new thread,
 *      then this problem will go away.
 */
//##ModelId=3B0753810280
void ServerConnection::run() {
  assert(rsocket != NL_INVALID);
	//Do connection negotiaion here, and create the PacketStream
  onNewConn();
}

//##ModelId=3B0753810283
void ServerConnection::onConnFailure(Connection::FailureType errorType) {
}

}


