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
  //If this is the current thread, we did a detach(true) because we failed 
  //before onNewConn.  Else this is another thread (likely our event thread)
  //and therefore we call join to wait for run to complete if it is still
  //running (as it would be if an error occured after onNewConn but before
  //run finished), and join will also release thread resources.
  if (Thread::currentThread() != this)
    join();
  gnedbgo(5, "destroyed");
}

/**
 * \todo implement negotiation
 */
//##ModelId=3B0753810280
void ServerConnection::run() {
  assert(sockets.r != NL_INVALID);
  assert(eventListener != NULL);
  gnedbgo1(1, "New connection incoming from %s", getRemoteAddress(true).toString().c_str());

  bool onNewConnFinished = false;
  SyncConnection sConn(this);
  try {
    sConn.startConnect();
    ps->start();
    connecting = true;
    eventListener->start();
    reg(true, false);

    //Do GNE protocol connection negotiaion here

    gnedbgo2(2, "Starting onNewConn r: %i, u: %i", sockets.r, sockets.u);
    getListener()->onNewConn(sConn); //SyncConnection will relay this
    onNewConnFinished = true;
    finishedConnecting();

    //Start bringing connection to normal state.  SyncConnection will make
    //sure onDisconnect gets called starting with endConnect().
    sConn.endConnect(true);
    sConn.release();
  } catch (Error e) {
    if (!onNewConnFinished) {
      sConn.endConnect(false);
      ourCreator->onListenFailure(e, getRemoteAddress(true), getListener());
      //We delete ourselves when we terminate since we were never seen by the
      //user and no one has seen us yet.
      detach(true);
    }
  }
}

}
