#ifndef _SERVERCONNECTIONLISTENER_H_
#define _SERVERCONNECTIONLISTENER_H_

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
#include "ServerConnectionCreator.h"
#include "Connection.h"

/**
 * A connection that listens for other connections.  Inherit from this class,
 * overriding the functions from Connection and ClientConnection based on the
 * events you wish to respond to.
 */
//##ModelId=3B01D3D7006E
class ServerConnectionListener {
public:
  /**
   * Initalizes this class.
   * @param outRate max outbandwith per client
   * @param inRate max clients can send to us.
   * @param port the port number to listen on.
   * @param creator2 the class to create your custom ServerConnection objects.
   *                 This object will be destroyed with the class.
   */
  //##ModelId=3B01D3D70208
  ServerConnectionListener(int outRate2, int inRate2, ServerConnectionCreator* creator2);

  /**
   * The destructor will close down a listening object, so when you are done
   * listening, just destroy this object.
   */
  //##ModelId=3B01D3D7020B
  virtual ~ServerConnectionListener();

  /**
   * Opens a socket ready for listening, but not yet listening.
   * @param port the port to listen on.
   * @return true if could not open a socket on the port.
   */
  //##ModelId=3B01D3D70227
  bool open(int port);

  /**
   * Starts this socket listening.  onNewConn will be called when a new
   * connection has been negotiated and error checked.  When you are
   * finished, delete this object and the dtor will clean things up.
   * @see onListenFailure
   * @see #onNewConn(Connection*)
   * @return true, if there was an error.
   */
  //##ModelId=3B01D3D70229
  bool listen();

  /**
   * Returns true if this ServerConnectionListener is active.
   */
  //##ModelId=3B0334A90230
  bool isListening() const;

  /**
   * There was a failure when trying to listen on this socket.  This is not
   * called when the actual low-level listen fails (that error is returned
   * from listen), but instead high-level errors while connecting such as a
   * version mismatch are sent here.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   * @param errorType the type of error
   */
  //##ModelId=3B0334A90348
  virtual void onListenFailure(Connection::FailureType errorType);

  /**
   * Returns the address of the listening socket.
   */
  //##ModelId=3B01D3D70230
  NLaddress getLocalAddress() const;

private:
  //##ModelId=3B01D3D7006F
  class ServerListener : public ConnectionEventListener {
  public:
    //##ModelId=3B01D3D701FA
    ServerListener(ServerConnectionListener& listener);

    //##ModelId=3B01D3D701FC
    virtual ~ServerListener();

    //##ModelId=3B01D3D701FE
    void onReceive();

  private:
    //##ModelId=3B01D3D701F6
    ServerConnectionListener& conn;

  };
  friend class ServerListener;

  //##ModelId=3B01D3D70232
  void onReceive();

  //##ModelId=3B0334A900E6
  bool listening;

  //##ModelId=3B01D3D70202
  ServerListener* listener;

  //##ModelId=3B01D3D70206
  int inRate;

  //##ModelId=3B01D3D70207
  int outRate;

  //##ModelId=3B01D48100DC
  NLsocket socket;

  //##ModelId=3B01E1F40227
  ServerConnectionCreator* creator;
};

#endif

