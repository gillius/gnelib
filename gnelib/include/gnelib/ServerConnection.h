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
class ConnectionListener;
class ServerConnectionListener;

/**
 * A GNE "internal" class.  Users will use this class, but probably only as
 * its base class -- a Connection.  This class is created by
 * ServerConnectionListener when incoming connections are comming in.
 */
//##ModelId=3B0753800310
class ServerConnection : public Connection, public Thread {
public:
  /**
   * Intializes this class.
   * @see Connection#Connection(int, int, ConnectionListener*)
   * @param rsocket2 the reliable socket received from the accept command.
   * @param creator the ServerConnectionListener that created us, so that we
   *                may call its onListenFailure event.
   */
  //##ModelId=3B075381027A
  ServerConnection(int outRate, int inRate, ConnectionListener* listener, 
                   NLsocket rsocket2, ServerConnectionListener* creator);

  /**
   * Destructs this ServerConnection object.  The user need not worry about
   * the fact that ServerConnection is a thread (in the sense that the user
   * never need to call detach or join), to do a proper cleanup.
   */
  //##ModelId=3B075381027E
  virtual ~ServerConnection();

protected:
  /**
   * This thread performs the connection process.  If an error occurs:\n
   * Before onNewConn: ServerConnectionListener::onListenFailure() is called.\n
   * During onNewConn: Only onNewConn is called and is reponsible for catching
   *                   the exception and cleaning up anything it has done.
   *                   onDisconnect will NOT be called, but onListenFailure
   *                   will be.
   * After onNewConn:  onDisconnect will be called.  onFailure will likely not
   *                   be called first.
   */
  //##ModelId=3B0753810280
  void run();

private:
  /**
   * Used only while connecting to transmit an error if necessary.
   * @see run
   */
  //##ModelId=3BCFAE590227
  ServerConnectionListener* ourCreator;
};

}
#endif /* SERVERCONNECTION_H_INCLUDED_C4FE6FF3 */


