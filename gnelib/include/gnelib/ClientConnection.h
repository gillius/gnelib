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

#include "Connection.h"
#include "Thread.h"
#include "Address.h"
#include "SmartPtr.h"
#include "WeakPtr.h"

namespace GNE {
class ConnectionListener;
class Error;
class ClientConnectionParams;
class ConnectionParams;
class SyncConnection;

/**
 * @ingroup midlevel
 *
 * A class that can connect to a remote target.
 */
class ClientConnection : public Connection, public Thread {
protected:
  /**
   * Initializes this ClientConnection.  The listener is passed in the open
   * method.
   */
  ClientConnection();

public:
  typedef SmartPtr<ClientConnection> sptr;
  typedef WeakPtr<ClientConnection> wptr;

  /**
   * Creates a new ClientConnection object, managed by a SmartPtr.
   */
  static sptr create();

  virtual ~ClientConnection();

  /**
   * Opens the socket, ready for connect, but does not yet connect.  All of
   * the relvant parameters for establishing the connection are passed into
   * this function.  If there is already an error, the function returns true.
   *
   * @param dest the destination address.
   */
  bool open(const Address& dest, const ConnectionParams& p);

  /**
   * Starts connection to the specified target.  This method does not block,
   * and a thread will be started to handle the connection process.
   * onConnect() or onConnectFailure() will be called depending on the
   * outcome of this process.  The exception is that if an error occurs
   * during onConnect or it chooses to reject the connection,
   * onConnectFailure will also be called.
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
   * @see ConnectionListener#onConnect()
   * @see ConnectionListener#onConnectFailure()
   */
  void connect();
  
private:
  friend class SyncConnection;
  //This method only to be called by SyncConnection
  //raw pointer we know is OK since SyncConnection joins on this, the raw ptr
  //is guaranteed to be valid through the whole connect process.
  void connect( const SmartPtr<SyncConnection>& );

protected:
  /**
   * Connection starts a new thread lasting only while it is connecting.
   * @see Thread#run()
   */
  virtual void run();

private:
  /**
   * Does the GNE protocol handshake, throwing an Error on error.
   */
  void doHandshake();

  /**
   * Sends the CRP, throwing an Error on error.
   */
  void sendCRP();

  /**
   * Tries to get the CAP (or refusal packet), throwing an Error on error or
   * if the connection was refused.
   * @return an address to connect to the remote unreliable connection.  If
   *         no unreliable connection was requested, the value of the
   *         returned address is undefined.
   */
  Address getCAP();

  /**
   * Sets up the unreliable connection, throwing an Error on error.
   */
  void setupUnreliable(const Address& dest);

  //Temp storage of connection params.
  ClientConnectionParams* params;
};

}
#endif /* CLIENTCONNECTION_H_INCLUDED_C51A7F46 */
