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
class Error;
class ClientConnectionParams;

/**
 * A class that can connect to a remote target.
 */
//##ModelId=3B0753800270
class ClientConnection : public Connection, public Thread {
public:
  /**
   * Initializes this ClientConnection.
   * @see Connection#Connection(ConnectionListener*)
   */
  //##ModelId=3B075380037F
  ClientConnection(ConnectionListener* listener = NULL);

  //##ModelId=3B07538003B8
  virtual ~ClientConnection();

  /**
   * Opens the socket, ready for connect, but does not yet connect.  All of
   * the relvant parameters for establishing the connection are passed into
   * this function.  If there is already an error, the function returns true.
   * @param dest the destination address.
   * @param outRate the maximum rate in bytes per second to send.  If this is
   *        0, then the rate is unlimited.
   * @param inRate the maximum rate we allow the sender to send to us in
   *        bytes per second.  If this is 0, then the requested
   *        incoming rate has no bounds.
   * @param localPort local port to open on, default 0 for don't care.
   * @param wantUnreliable true if we want to use a lower-overhead connection
   *        for sending unreliable packets.
   * @return true if the socket could not be opened, if dest, outRate,
   *              inRate, or localPort have invalid values.
   */
  //##ModelId=3B07538003BB
  bool open(const Address& dest,
            int outRate = 0,
            int inRate = 0,
            int localPort = 0,
            bool wantUnreliable = true);

  /**
   * Starts connection to the specified target.  This method does not block,
   * and a thread will be started to handle the connection process.
   * onConnect() or onConnectFailure() will be called depending on the
   * outcome of this process.  The exception is that if an error occurs
   * during onConnect or it chooses to reject the connection,
   * onConnectFailure will also be called.
   *
   * You must call either detach(false) or join on this object after calling
   * connect, depending if you want to wait for the connection to complete.
   *
   * You can call join after connect to wait until the connection is
   * finished, when onConnect or onConnectFailure will be called.  When the
   * called function exits, the thread will stop and join will return.  If
   * you do not need asyncronous sockets, you do not need to overload these
   * functions and can rely on the isConnected method.  You still need to
   * overload onConnectFailure to get a description of the error, however.
   *
   * The version number for the library and your own version number will be
   * checked during this phase.  If either protocol versions mismatch,
   * onConnectFailure() will be triggered.
   *
   * If sConn is not null, then there is currently a SyncConnection wrapped
   * around this connection.  This is only the case when a user is using only
   * SyncConnections, and in this case the user should call
   * SyncConnection::connect(), so in your code you should never need to pass
   * a parameter to this function.
   *
   * @see ConnectionListener#onConnect()
   * @see ConnectionListener#onConnectFailure()
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
   * Does the GNE protocol handshake, throwing an Error on error.
   */
  //##ModelId=3C783ACF0192
  void doHandshake();

  /**
   * Sends the CRP, throwing an Error on error.
   */
  //##ModelId=3C5CED05016E
  void sendCRP();

  /**
   * Tries to get the CAP (or refusal packet), throwing an Error on error or
   * if the connection was refused.
   * @return an address to connect to the remote unreliable connection.  If
   *         no unreliable connection was requested, the value of the
   *         returned address is undefined.
   */
  //##ModelId=3C5CED05016F
  Address getCAP();

  /**
   * Sets up the unreliable connection, throwing an Error on error.
   */
  //##ModelId=3C5CED050170
  void setupUnreliable(const Address& dest);

  //Temp storage of connection params.
  //##ModelId=3C5CED05013D
  ClientConnectionParams* params;
};

}
#endif /* CLIENTCONNECTION_H_INCLUDED_C51A7F46 */



