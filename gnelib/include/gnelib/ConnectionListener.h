#ifndef CONNECTIONLISTENER_H_INCLUDED_C43554C0
#define CONNECTIONLISTENER_H_INCLUDED_C43554C0

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

namespace GNE {
class Error;
class SyncConnection;

/**
 * This is an abstract base class representing an interface to the Connection,
 * ClientConnection, and ServerConnection classes.  All of the events that
 * those three classes generate are in this class, and classes listening to
 * the events on these classes will inherit from this class.\n
 * Override the events you want to receive with this listener.  The provided
 * functions do nothing, so that event overloading is optional in all cases.\n
 * onConnect and onConnectFailure are generated only by ClientConnection and
 * onNewConn is generated only by ServerConnection.
 * @see ServerConnectionListener::onListenFailure
 */
//##ModelId=3BCA810C0276
class ConnectionListener {
public:
  //##ModelId=3BCA817C0046
  virtual ~ConnectionListener();

  /**
   * A Connection must always have a non-NULL listener if it is to be used,
   * but if you will only be using a SyncConnection for the whole time, there
   * is no need to create your own listener.  This listener will ignore all
   * events.
   */
  //##ModelId=3C09B42D0144
  static ConnectionListener* getNullListener();

  /**
   * Event triggered after there is a successful connection.  The connection
	 * process will not be considered complete until this function completes.
	 * \nNote: Only ClientConnection generates this event.  The SyncConnection
	 * is currently wrapped around a ClientConnection, and you should use
	 * SyncConnection::getConnection to get the ClientConnection.
   * \nRemember that conn will trap your events like onReceive and
   * onDoneWriting.  So if you will not be using conn, you should release it
   * if you are writing and reading using the PacketStream only because you
   * will want to get onDoneWriting most likely.  onDoneWriting and
   * onReceive can be called BEFORE onConnect finishes in this case if you
   * release!  After onConnect finishes, conn will be released regardless of
   * what you did with it or if you released it.
   * \nIn the near future the ability to refuse connections will be added.
   */
  //##ModelId=3BCA83910078
  virtual void onConnect(SyncConnection& conn);

  /**
   * Event triggered when a connection failed.\n
	 * After a connection failure, the connection is as if disconnect() was
	 * called, therefore you cannot reconnect this connection instance.\n
	 * Note that for this case, the event onDisconnect IS NOT CALLED, since
	 * the Connection was never in a connected state.
	 * \nNote: Only ClientConnection generates this event.
	 * @see Connection::disconnect()
   */
   //##ModelId=3BCA83920262
  virtual void onConnectFailure(const Error& error);

  /**
   * Event triggered when a new connection has been negotiated and error
   * checked.  This object is a newly allocated object created from your
   * ServerConnectionCreator object, and this function will be the first time
   * you code has "seen" this object, so you will have to register it into
   * some internal list so you can interact with and delete it later.\n
	 * Note: Only ServerConnection generates this function.  The SyncConnection
	 * is currently wrapped around a ServerConnection, and you should use
	 * SyncConnection::getConnection to get the ServerConnection.\n
   * If the connection failed, though, the function
	 * ServerConnectionListener::onListenFailure instead of this function is
	 * called.
   * \nRemember that newConn will trap your events like onReceive and
   * onDoneWriting.  So if you will not be using newConn, you should release
   * it if you are writing and reading using the PacketStream only because
   * you will want to get onDoneWriting most likely.  onDoneWriting and
   * onReceive can be called BEFORE onConnect finishes in this case if you
   * release!  After onConnect finishes, conn will be released regardless of
   * what you did with it or if you released it.
   * \nIn the near future the ability to refuse connections will be added.
   */
  //##ModelId=3BCFAE5900AA
  virtual void onNewConn(SyncConnection& newConn);

	/**
	 * An event triggered when a socket is closing for any reason.  This event
	 * is always called once and only once if a socket was connected.  At the
	 * time this event is called, the sockets are still connected, so you can
	 * get their address (for logging and/or reporting reasons), but you cannot
	 * send any more data or receive any from this event.\n
	 * onDisconnect will always be the last event called on this listener, so
	 * you can destroy this object after onDisconnect is called and it is not
	 * listening for any other connections.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
	 * @see ~Connection()
	 */
  //##ModelId=3BCA83BA02F8
  virtual void onDisconnect();

	/**
	 * This event is triggered when a non-fatal error occurs in a connection
	 * that does not force the connection to close, for example an unknown
	 * packet encounted is an error, but the connection can still proceed.\n
	 * After this event is processed, connections resume normally.
	 * Connection::disconnect() may also be called at this point if you wish
	 * to terminate the connection anyways.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
	 * @see onFailure()
	 * @see disconnect()
   */
  //##ModelId=3BCA83C600B4
  virtual void onError(const Error& error);

  /**
   * This event is triggered when a fatal error occurs in a connection.
	 * When a fatal error occurs, communications cannot contiune and the
	 * socket will be disconnected.  An onDisconnect() event will occur
	 * immediately after this event completes.  Most errors in GNE are fatal.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
	 * @see onError()
   */
  //##ModelId=3BCA83C803A2
  virtual void onFailure(const Error& error);

  /**
   * Event triggered when one or more packets have been recieved.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
   */
  //##ModelId=3BCA83CF0208
  virtual void onReceive();

  /**
   * Event triggered when the write buffer was filled and is now empty.
   * Note that this does not mean that data is immediately ready to be sent
   * again -- there could still be a flow control delay.\n
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single writer thread per connection.  Therefore, no more
	 * writing will take place until this function completes.
   */
  //##ModelId=3BCA83D101F4
  virtual void onDoneWriting();

};

} // namespace GNE



#endif /* CONNECTIONLISTENER_H_INCLUDED_C43554C0 */
