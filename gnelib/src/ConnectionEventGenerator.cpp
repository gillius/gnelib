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
#include "ConnectionEventGenerator.h"
#include "ConnectionEventListener.h"
#include "ConditionVariable.h"
#include "Connection.h"

namespace GNE {

//##ModelId=3B07538100B9
ConnectionEventGenerator::ConnectionEventGenerator() 
: group(NL_INVALID) {
  group = nlGroupCreate();
  assert(group != NL_INVALID);
  sockBuf = new NLsocket[NL_MAX_GROUP_SOCKETS];
}

//##ModelId=3B07538100BA
ConnectionEventGenerator::~ConnectionEventGenerator() {
  nlGroupDestroy(group);
  delete[] sockBuf;
}

/**
 * \bug I think it might be possible for a syncronization error to occur
 *      where an event can be generated on a Connection that was JUST
 *      shutdown.
 */
//##ModelId=3B07538100BC
void ConnectionEventGenerator::run() {
  while (!shutdown) {
    mapCtrl.acquire();
    while (connections.empty() && !shutdown) {
      mapCtrl.wait();
    }
    if (!shutdown) {
      int numsockets = nlPollGroup(group, NL_READ_STATUS, sockBuf, NL_MAX_GROUP_SOCKETS);
      for (numsockets--; numsockets >= 0; numsockets--) {
        connections[sockBuf[numsockets]]->onReceive();
      }
    }
    mapCtrl.release();
  }
}

//##ModelId=3B07538100BD
void ConnectionEventGenerator::reg(NLsocket socket, ConnectionEventListener* conn) {
  mapCtrl.acquire();
  nlGroupAddSocket(group, socket);
  connections[socket] = conn;
  mapCtrl.release();
  mapCtrl.signal();
}

//##ModelId=3B07538100DD
void ConnectionEventGenerator::unreg(NLsocket socket) {
  assert(socket != NL_INVALID);
  assert(connections.find(socket) != connections.end());
  mapCtrl.acquire();
  nlGroupDeleteSocket(group, socket);
  connections.erase(socket);
  mapCtrl.release();
}

//##ModelId=3B07538100DF
void ConnectionEventGenerator::shutDown() {
  mapCtrl.signal();
  Thread::shutDown();
}

}