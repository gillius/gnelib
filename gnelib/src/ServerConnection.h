#ifndef SERVERCONNECTION_H_INCLUDED_C4FE6FF3
#define SERVERCONNECTION_H_INCLUDED_C4FE6FF3

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
#include "Connection.h"
#include "Thread.h"

namespace GNE {

/**
 * The server-side connection.  Once it is connected, there's very little to
 * no difference between ClientConnection and ServerConnection at this level.
 */
//##ModelId=3B0753800310
class ServerConnection : public Connection, public Thread {
public:
  /**
   * Intializes this class, given the flow control parameters.
   * @param outRate the maximum rate in bytes per second to send.
   * @param inRate the maximum rate we allow the sender to send to us in
   *        bytes per second.
   */
  //##ModelId=3B075381027A
  ServerConnection(int outRate, int inRate, NLsocket rsocket2);

  //##ModelId=3B075381027E
  virtual ~ServerConnection();

  /**
   * Don't call this function directly.  This thread performs the connection
   * process.  The thread will have finished before onNewConn is called.
   */
  //##ModelId=3B0753810280
  void run();

  /**
   * Event triggered when a new connection has been negotiated and error
   * checked.  This object is a newly allocated object created from your
   * ServerConnectionCreator object, and this function will be the first time
   * you code has "seen" this object, so you will have to register it into
   * some internal list so you can interact with and delete it later.  This
   * functionality could exist in your ServerConnectionCreator::create(),
   * though rather than in this function.\n
   * If the connection failed, though, the function onConnFailure instead of
   * this function is called.\n
   */
  //##ModelId=3B0753810281
  virtual void onNewConn() = 0;

  /**
   * Event triggered when the connection failed.  Either this function or
   * onNewConn will be called when a new connection is made.  If you want to
   * get rid of this object right now, call detach(true) in this function. If
   * you want the object to stick around, like onNewConn, you must save a
   * pointer elsewhere so you can interact with and delete this object later.
   * @see onNewConn
   */
  //##ModelId=3B0753810283
  virtual void onConnFailure(FailureType errorType) = 0;

};

}
#endif /* SERVERCONNECTION_H_INCLUDED_C4FE6FF3 */


