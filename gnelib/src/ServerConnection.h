#ifndef SERVERCONNECTION_H_INCLUDED_C51A4E36
#define SERVERCONNECTION_H_INCLUDED_C51A4E36

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
#include "ConnectionEventListener.h"
#include "Connection.h"

/**
 * A connection that listens for other connections.  Inherit from this class,
 * overriding the functions from Connection and ClientConnection based on the
 * events you wish to respond to.
 */
//##ModelId=3AE59EF1015E
class ServerConnection {
public:
  /**
   * Initalizes this class.
   * @param outRate max outbandwith per client
   * @param inRate max clients can send to us.
   * @param port the port number to listen on.
   */
  //##ModelId=3AE59FAF033E
  ServerConnection(int outRate, int inRate);

  //##ModelId=3AE59FAF037A
  virtual ~ServerConnection();

  /**
   * Opens a socket ready for listening, but not yet listening.
   * @param port the port to listen on.
   * @return true if could not open a socket on the port.
   */
  //##ModelId=3B009F57037A
  bool open(int port);

  /**
   * Starts this socket listening.  onNewConn will be called when a new
   * connection has been negotiated and error checked.
   * @see onListenFailure
   * @see #onNewConn(Connection*)
   * @return true, if there was an error.
   */
  //##ModelId=3AE5A0F60028
  bool listen();

  /**
   * Event triggered when a new connection has been negotiated and error
   * checked.
   * @param newConn the new Connection created.  You are responsible for
   *                deallocating this new Connection.
   */
  //##ModelId=3AE5A10D00A0
  virtual void onNewConn(Connection* newConn) = 0;

  /**
   * There was a failure when trying to listen on this socket.  This is not
   * called when the actual low-level listen fails (that error is returned
   * from listen), but instead high-level errors while connecting such as a
   * version mismatch are sent here.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   * @param errorType the type of error
   */
  //##ModelId=3AE5A1310208
  virtual void onListenFailure(Connection::FailureType errorType);

  /**
   * Returns the address of the listening socket.
   */
  //##ModelId=3B00A3BE0352
  NLaddress getLocalAddress() const;

private:
  //##ModelId=3B009B9E02C6
  class ServerListener : public ConnectionEventListener {
  public:
    //##ModelId=3B009DCD03C0
    ServerListener(ServerConnection& listener);

    //##ModelId=3B009DCE0046
    virtual ~ServerListener();

    //##ModelId=3B009BAA00F0
    void onReceive();

  private:
    //##ModelId=3B009DB1029E
    ServerConnection& conn;

  };
  friend class ServerListener;

  //##ModelId=3AE5AF95021C
  void onReceive();

  //##ModelId=3B009AE2032A
  NLsocket socket;

};



#endif /* SERVERCONNECTION_H_INCLUDED_C51A4E36 */


