#ifndef CLIENTCONNECTION_H_INCLUDED_C51A7F46
#define CLIENTCONNECTION_H_INCLUDED_C51A7F46

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
#include "Address.h"

namespace GNE {
class ConnectionListener;

/**
 * A class that can connect to a remote target.
 */
//##ModelId=3B0753800270
class ClientConnection : public Connection, public Thread {
public:
  /**
   * Initializes this ClientConnection.
   * @see Connection#Connection(int, int, ConnectionListener*)
   */
  //##ModelId=3B075380037F
  ClientConnection(int outRate, int inRate, ConnectionListener* listener = NULL);

  //##ModelId=3B07538003B8
  virtual ~ClientConnection();

  /**
   * Opens the socket, ready for connect, but does not yet connect.
   * @param localPort local port to open on, default 0 for don't care.
   * @return true if the socket could not be opened.
   */
  //##ModelId=3B07538003BB
  bool open(const Address& dest, int localPort = 0);

  /**
   * Starts connection to the specified target.  This method does not block,
   * and a thread will be started to handle the connection process.
   * onConnect() or onConnectFailure() will be called depending on the
   * outcome of this process.\n
   * You must call either detach(false) or join on this object after calling
   * connect, depending if you want to wait for the connection to complete.\n
   * You can call join after connect to wait until the connection is
   * finished, when onConnect or onConnectFailure will be called.  When the
   * called function exits, the thread will stop and join will return.  If
   * you do not need asyncronous sockets, you do not need to overload these
   * functions and can rely on the isConnection function.  You still need to
   * overload onConnectFailure to get a description of the error, however.\n
   * The version number for the library and your own version number will be
   * checked during this phase.  If either protocol versions mismatch,
   * onConnectFailure() will be triggered.\n
   * If sConn is not null, then there is currently a SyncConnection wrapped
   * around this connection.  This is only the case when a user is using only
   * SyncConnections.  So the user should always pass nothing to this
   * function -- this parameter is only useful in the code for
   * SyncConnection::connect().
   * @see #onConnect()
   * @see #onConnectFailure()
   */
  //##ModelId=3B07538003C1
  void connect(SyncConnection* wrapped = NULL);

protected:
  /**
   * Connection starts a new thread lasting only while it is connecting.
   * @see Thread#run()
   */
  //##ModelId=3B07538003BA
  void run();

private:
  /**
   * These two variables used only while connecting, then unused afterwards.
   */
  //##ModelId=3BB94B19035D
  Address address;
  SyncConnection* sc;
};

}
#endif /* CLIENTCONNECTION_H_INCLUDED_C51A7F46 */



