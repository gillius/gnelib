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
#include "../include/gnelib/ServerConnection.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/ServerConnectionListener.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/EventThread.h"

namespace GNE {

//##ModelId=3B075381027A
ServerConnection::ServerConnection(int outRate, int inRate, 
                                   ConnectionListener* listener, 
                                   NLsocket rsocket2, 
                                   ServerConnectionListener* creator)
: Connection(outRate, inRate, listener), Thread("SrvrConn"),
ourCreator(creator) {
  gnedbgo(5, "created");
  sockets.r = rsocket2;
}

//##ModelId=3B075381027E
ServerConnection::~ServerConnection() {
  gnedbgo(5, "destroyed");
}

/**
 * \todo implement negotiation
 */
//##ModelId=3B0753810280
void ServerConnection::run() {
  assert(sockets.r != NL_INVALID);
  gnedbgo2(2, "connection r: %i, u: %i", sockets.r, sockets.u);

  assert(eventListener != NULL);
  connecting = true;
  try {
    SyncConnection sync(this);
    eventListener->start();
    reg(true, false);
    ps->start();
    //Do connection negotiaion here
    getListener()->onNewConn(sync); //SyncConnection will relay this
    sync.release();
    finishedConnecting();
  } catch (Error e) {
    ourCreator->onListenFailure(e, getRemoteAddress(true), getListener());
  }
}

}





