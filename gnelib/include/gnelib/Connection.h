#ifndef CONNECTION_H_INCLUDED_C51DC478
#define CONNECTION_H_INCLUDED_C51DC478

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
#include "Thread.h"
#include "PacketStream.h"
#include "ConnectionEventListener.h"
#include "ErrorGne.h"
#include "SocketPair.h"

namespace GNE {

/**
 * A class resembling any type of connection to a remote computer.  A
 * connection also manages flow control.  When a new Connection instance is
 * constructed the passed inRate and outRate parameters set a maximum
 * bandwith usage for either direction.  These values might stay the same or
 * reduce after a connection to the remote machine is made, based on that
 * machine's requested maximums.
 */
//##ModelId=3B075380029E
class Connection {
public:
  /**
   * A struct holding values returned by getStats.
   */
  //##ModelId=3B075380029F
  struct Stats {
    //##ModelId=3B0753810008
    int sent;
    //##ModelId=3B0753810009
    int recv;
    //##ModelId=3B075381000A
    int maxAvgSent;
    //##ModelId=3B075381000B
    int maxAvgRecv;
    //##ModelId=3B075381000C
    int avgSent;
    //##ModelId=3B075381003C
    int avgRecv;
  };

  /**
   * Intializes this class, given the flow control parameters.
   * @param outRate the maximum rate in bytes per second to send.
   * @param inRate the maximum rate we allow the sender to send to us in
   *        bytes per second.
   */
  //##ModelId=3B0753810073
  Connection(int outRate, int inRate);

	/**
	 * A Connection will automatically disconnect if it is connected when it
	 * is destructed.
	 */
  //##ModelId=3B0753810076
  virtual ~Connection();

  /**
   * Returns the PacketStream associated with this class.  Writing and
   * reading through this Connection is done through this PacketStream.
   */
  //##ModelId=3B0753810078
  PacketStream& stream();

  /**
   * If stats is enabled, returns Connection stats.
   * @return various connection stats.
   * @see ConnectionStats
   */
  //##ModelId=3B0753810079
  Stats getStats() const;

  /**
	 * \bug result is undefined when the underlying socket isn't connected,
	 *      but the user has no way to know if this function failed due to
	 *      this case, unless the user has an intimate understanding of the
	 *      implementation of Connection.
   * Returns the local address of this connection.
   * @param reliable sometimes two sockets are used for reliable and
   *                 unreliable data.  Specify which low-level socket you
   *                 want the address of.
   */
  //##ModelId=3B075381007B
  NLaddress getLocalAddress(bool reliable) const;

  /**
	 * \bug result is undefined when the underlying socket isn't connected,
	 *      but the user has no way to know if this function failed due to
	 *      this case, unless the user has an intimate understanding of the
	 *      implementation of Connection.
   * Returns the remote address of this connection, if it is connected.
   * @param reliable sometimes two sockets are used for reliable and
   *                 unreliable data.  Specify which low-level socket you
   *                 want the address of.
   */
  //##ModelId=3B075381007E
  NLaddress getRemoteAddress(bool reliable) const;

  /**
   * Returns true if this Connection is active and ready to send/recieve.
   */
  //##ModelId=3B0753810081
  bool isConnected() const;

  /**
   * Immediately disconnects this socket.  No more data will be recieved or
   * sent on this socket.  If you want to disconnect more nicely, use
   * disconnectSendAll.  It is okay to call this function even when this
	 * Connection is already disconnected.\n
	 * NOTE: You may not reconnect this connection object after calling
	 * disconnect.
   * @see #disconnectSendAll()
   */
  //##ModelId=3B0753810083
  void disconnect();

  /**
   * A blocking call that disconnects nicely.  This has the same effect as
   * disconnect, except that all data waiting to be sent will be sent if
   * possible.  If the connection is already disconnected, this function does
	 * nothing.
   * @see #disconnect()
   */
  //##ModelId=3B0753810084
  void disconnectSendAll();

	/**
	 * An event triggered when a socket is closing for any reason.  This event
	 * is always called once and only once if a socket was connected.  At the
	 * time this event is called, the sockets are still connected, so you can
	 * get their address (for logging and/or reporting reasons), but you cannot
	 * send any more data or receive any from this event.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
	 */
	virtual void onDisconnect();

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
  //##ModelId=3B0753810085
  virtual void onFailure(const Error& error);

	/**
	 * This event is triggered when a non-fatal error occurs in a connection
	 * that does not force the connection to close, for example an unknown
	 * packet encounted is an error, but the connection can still proceed.\n
	 * After this event is processed, connections resume normally.
	 * disconnect() may also be called at this point if you wish to terminate
	 * the connection anyways.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
	 * @see onFailure()
	 * @see disconnect()
   */
	virtual void onError(const Error& error);

  /**
   * Event triggered when one or more packets have been recieved.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single event thread.  Therefore, some events for any 
	 * connection will not be called until this function completes.
   */
  //##ModelId=3B07538100AC
  virtual void onReceive();

  /**
   * Event triggered when the write buffer was filled and is now empty.
   * Note that this does not mean that data is immediately ready to be sent
   * again -- there could still be a flow control delay.\n
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
	 * \nThis event must be "non-blocking" -- like most GNE events -- as there
	 * is only a single writer thread per connection.  Therefore, no more
	 * writing will take place until this function completes.
   */
  //##ModelId=3B07538100AE
  virtual void onDoneWriting();

protected:
	/**
	 * The pair of sockets.
	 */
  //##ModelId=3BB2CB410263
	SocketPair sockets;

  /**
   * The PacketStream associated with this Connection.  This object also
   * contains information about the in and out connection rates.
   */
  //##ModelId=3B075381006E
  PacketStream* ps;

  /**
   * Is this Connection active and ready for communications?
   */
  //##ModelId=3B0753810072
  bool connected;

	/**
	 * Register this Connection object's sockets with the
	 * ConnectionEventGenerator.  Pass true to register each socket type.
	 * Usually both will be registered, but in the future there may exist a
	 * new connection type that only uses one of the sockets.\n
	 * The internal class ConnectionListener will be used to trigger the proper
	 * onReceive(bool) event.\n
	 * The sockets will be unregistered automatically when the object is
	 * destroyed, but they may be unregistered explicitly at an earlier time
	 * through unreg(bool, bool).\n
	 * Registering a socket already registered will have no effect.
	 * @see #onReceive(bool)
	 * @see #unreg(bool, bool)
	 */
  //##ModelId=3B6E14AC0104
	void reg(bool reliable, bool unreliable);

	/**
	 * Unregistered the sockets which are specified by passing true to the
	 * appropriate parameter.  This function call is optional -- when the
	 * object is destroyed, unregistering is automatic if socket was
	 * registered.  Trying to unregister sockets that are already unregistered
	 * will result in no effect.
	 */
  //##ModelId=3B6E14AC01D6
	void unreg(bool reliable, bool unreliable);

private:
  //##ModelId=3B07538002A0
  class ConnectionListener : public ConnectionEventListener {
  public:
    //##ModelId=3B075381004E
    ConnectionListener(Connection& listener, bool isReliable);

    //##ModelId=3B0753810051
    virtual ~ConnectionListener();

    //##ModelId=3B0753810053
    void onReceive();

  private:
    //##ModelId=3B0753810049
    Connection& conn;

    //##ModelId=3B075381004D
    bool reliable;

  };
	/**
	 * Make ConnectionListener a friend so it can call our onRecieve(bool)
	 * event, which will properly parse the packets.
	 */
  friend class ConnectionListener;

	//Possible ConnectionListeners that may or may not exist, but if they do we
	//need to kill them on exit.
  //##ModelId=3B6E14AC00FB
	ConnectionListener* rlistener;
  //##ModelId=3B6E14AC0100
	ConnectionListener* ulistener;

  friend class PacketStream;

	/**
	 * Parses the packets recieved, then calls onReceive.
	 */
  //##ModelId=3B07538100B0
  void onReceive(bool reliable);

	/**
	 * Determines whether the error given is fatal or non-fatal, and calls the
	 * appropriate event, and handles disconnects if necessary.
	 */
	void processError(const Error& error);
};

}
#endif /* CONNECTION_H_INCLUDED_C51DC478 */


