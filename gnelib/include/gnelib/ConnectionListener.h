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
 * onNewConn is generated only by ServerConnection.\n
 * Note that there is only one event thread per Connection, so one one event
 * will be active at a time.  Take this into consideration that you may not
 * need to provide syncronization and mutexes.  onNewConn or onConnect or
 * onConnectFailure are guaranteed to be the first events from a connection,
 * and won't be called multiple times.  onDisconnect will always be the last
 * called (except after onConnectFailure, where there never was a connection)
 * and will only be called once.  Since only one event can be active at a
 * time, you will want to return quickly so other events can be processed.
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
   * \nIf an error occurs then the connection needs to be killed, so conn can
   * throw its Error outside your function.  Catch it if you need to clean up
   * anything you were doing, but remember to rethrow it.
   * \nNote: Only ClientConnection generates this event.  The SyncConnection
   * is currently wrapped around a ClientConnection, and you should use
   * SyncConnection::getConnection to get the ClientConnection.
   * \nYou should not release the SyncConnection.  You can still interact
   * with the PacketStream directly, though, but the events will be delayed
   * until this function completes.
   * \nIn the near future the ability to refuse connections will be added.
   */
  //##ModelId=3BCA83910078
  virtual void onConnect(SyncConnection& conn) throw (Error);

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
   * some internal list so you can interact with and delete it later.
   * \nIf an error occurs then the connection needs to be killed, so conn can
   * throw its Error outside your function.  Catch it if you need to clean up
   * anything you were doing, but remember to rethrow it.\n
   * Note: Only ServerConnection generates this function.  The SyncConnection
   * is currently wrapped around a ServerConnection, and you should use
   * SyncConnection::getConnection to get the ServerConnection.\n
   * If the connection failed, though, the function
   * ServerConnectionListener::onListenFailure instead of this function is
   * called.
   * \nYou should not release the SyncConnection.  You can still interact
   * with the PacketStream directly, though, but the events will be delayed
   * until this function completes.
   * \nIn the near future the ability to refuse connections will be added.
   */
  //##ModelId=3BCFAE5900AA
  virtual void onNewConn(SyncConnection& newConn) throw (Error);

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
   * is only a single event thread per connection.  Therefore, no other
   * events will be called until this function completes for this connection.
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
   * is only a single event thread per connection.  Therefore, no other
   * events will be called until this function completes for this connection.
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
   * is only a single event thread per connection.  Therefore, no other
   * events will be called until this function completes for this connection.
   * @see onError()
   */
  //##ModelId=3BCA83C803A2
  virtual void onFailure(const Error& error);

  /**
   * Event triggered when one or more packets have been recieved.  It is
   * possible to get more packets while in this function, but if packets
   * arrive during this event, it will be called again, so you don't have to
   * make sure they have all been processed (there is no sure way to do
   * that.).
   * \nThis event must be "non-blocking" -- like most GNE events -- as there
   * is only a single event thread per connection.  Therefore, no other
   * events will be called until this function completes for this connection.
   */
  //##ModelId=3BCA83CF0208
  virtual void onReceive();

  /**
   * Event triggered when the write buffer was filled and is now empty.
   * Note that this does not mean that data is immediately ready to be sent
   * again -- there could still be a flow control delay.\n
   * \nThis event must be "non-blocking" -- like most GNE events -- as there
   * is only a single event thread per connection.  Therefore, no other
   * events will be called until this function completes for this connection.
   */
  //##ModelId=3BCA83D101F4
  virtual void onDoneWriting();

};

} // namespace GNE

#endif /* CONNECTIONLISTENER_H_INCLUDED_C43554C0 */
