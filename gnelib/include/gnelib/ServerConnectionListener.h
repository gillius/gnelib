#ifndef _SERVERCONNECTIONLISTENER_H_
#define _SERVERCONNECTIONLISTENER_H_

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

#include "ReceiveEventListener.h"
#include "Connection.h"
#include "SmartPtr.h"
#include "WeakPtr.h"
#include "Mutex.h"

namespace GNE {
class Address;
class ConnectionListener;
class ServerConnection;
class ConnectionParams;

/**
 * A connection that listens for other connections.  Inherit from this class,
 * overriding the functions from Connection and ClientConnection based on the
 * events you wish to respond to.
 *
 * You need to create a static "create" method like other classes in GNE since
 * ServerConnectionListener is managed by a SmartPtr.
 */
class ServerConnectionListener {
public:
  typedef SmartPtr<ServerConnectionListener> sptr;
  typedef WeakPtr<ServerConnectionListener> wptr;

protected:
  /**
   * Initalizes this class.
   */
  ServerConnectionListener();

public:
  /**
   * Destructor.
   */
  virtual ~ServerConnectionListener();

  /**
   * Opens a socket ready for listening, but not yet listening.
   * @param port the port to listen on.
   * @return true if could not open a socket on the port.
   */
  bool open(int port);

  /**
   * Closes the listening connection.
   */
  void close();

  /**
   * Starts this socket listening.  onNewConn will be called when a new
   * connection has been negotiated and error checked.  When you are
   * finished, delete this object and the dtor will clean things up.
   *
   * @see onListenFailure
   * @return true, if there was an error.
   */
  bool listen();

  /**
   * Returns true if this ServerConnectionListener is active.
   */
  bool isListening() const;

  /**
   * Returns the address of the listening socket.  If the listener has not
   * been opened or is not listening, an invalid address is returned.
   */
  Address getLocalAddress() const;

protected:
  /**
   * You must call this from your create function BEFORE exiting it.
   */
  void setThisPointer( const sptr& thisPointer );

  /**
   * There was a failure when trying to listen on this socket.  This is not
   * called when the actual low-level listen fails (that error is returned
   * from listen), but instead high-level errors while connecting such as a
   * version mismatch are sent here.  Only errors that occur before or during
   * the ConnectionListener::onNewConn event are sent here.
   *
   * This function may be called from multiple threads at the same time, and
   * may block if it wishes; it does not have the "non-blocking" restriction
   * most GNE events have.
   *
   * Depending on when and under what circumstances the failure happened, in
   * any case, from might be an invalid address, and listener may be NULL.
   *
   * @param error the type of error
   * @param from the remote address the failed attempt came from
   * @param listener the unused ConnectionListener that was obtained from
   *                 getNewConnectionParams.  If it is NULL, then
   *                 getNewConnectionParams was never called.
   */
  virtual void onListenFailure(const Error& error, const Address& from,
                               ConnectionListener* listener) = 0;

  /**
   * This is an optional event to catch which is called after
   * ConnectionListener::onNewConn.  The default behavior for this event is
   * to do nothing, but you may overload it to catch it.
   *
   * This method may block, but until it completes the connection will be
   * marked as connecting and not as connected, and listener will not start
   * to receive any events (besides the onNewConn it just got) until this
   * function completes.
   *
   * See the expong example of a case where the onListenSuccess event can be
   * useful, where the same implementation using only onNewConn would be
   * harder to program and understand.
   *
   * @param listener The listener than you returned from
   *                 getNewConnectionParams.
   */
  virtual void onListenSuccess(ConnectionListener* listener);

  /**
   * A new connection is starting, and now we want the parameters for this
   * new connection.  The parameters passed should be modified to give
   * the new connection's flow control parameters and listener.
   *
   * The listener pointer will be returned to you through the socket failure
   * event (in this case it was never used, but it is returned in case you
   * need to delete it).  If the connection is successful you can get your
   * pointer back through the resulting ServerConnection class by using
   * Connection::getListener, or through the onListenSuccess event.
   *
   * This function can be called from multiple threads at the same time.
   */
  virtual void getNewConnectionParams(ConnectionParams& params) = 0;

private:
  //interface functions solely for ServerConnection
  friend class ServerConnection;

  //These methods relay the message on, locking the sync mutex.
  void processOnListenFailure( const Error& error, const Address& from,
                               ConnectionListener* listener);

  void processOnListenSuccess( ConnectionListener* listener );

private:
  class ServerListener : public ReceiveEventListener {
  public:
    typedef SmartPtr<ServerListener> sptr;
    typedef WeakPtr<ServerListener> wptr;

  public:
    ServerListener(const ServerConnectionListener::sptr& listener);

    virtual ~ServerListener();

    void onReceive();

  private:
    ServerConnectionListener::sptr conn;

  };
  friend class ServerListener;

  void onReceive();

  wptr this_;

  bool listening;

  NLsocket socket;

  mutable Mutex sync;
};

}
#endif
