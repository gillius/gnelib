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
   * An enum describing connection failure type.
   */
  //##ModelId=3B075381003D
  enum FailureType {
    NoError = 0,
    GNEHostVersionLow = 1,
    GNEHostVersionHigh = 2,
    UserHostVersionLow = 3,
    UserHostVersionHigh = 4,
    CouldNotOpenSocket = 5,
    ConnectionTimeOut = 6,
    OtherLowLevelError = 7
  };

  /**
   * An array of strings matching FailureType to a string.  Example:
   * FailureStrings[GNEHostVersionLow];
   */
  //##ModelId=3AE5BA8F038E
  static const std::string FailureStrings[];

  /**
   * Intializes this class, given the flow control parameters.
   * @param outRate the maximum rate in bytes per second to send.
   * @param inRate the maximum rate we allow the sender to send to us in
   *        bytes per second.
   */
  //##ModelId=3B0753810073
  Connection(int outRate, int inRate);

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
   * Returns the local address of this connection.
   * @param reliable sometimes two sockets are used for reliable and
   *                 unreliable data.  Specify which low-level socket you
   *                 want the address of.
   */
  //##ModelId=3B075381007B
  NLaddress getLocalAddress(bool reliable) const;

  /**
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
   * disconnectSendAll.
   * @see #disconnectSendAll()
   */
  //##ModelId=3B0753810083
  void disconnect();

  /**
   * A blocking call that disconnects nicely.  This has the same effect as
   * disconnect, except that all data waiting to be sent will be sent if
   * possible.
   * @see #disconnect()
   */
  //##ModelId=3B0753810084
  void disconnectSendAll();

  /**
   * This is called when something bad happens, such as a timeout.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3B0753810085
  virtual void onFailure(FailureType errorType);

  /**
   * Event triggered when one or more packets have been recieved.
   */
  //##ModelId=3B07538100AC
  virtual void onReceive();

  /**
   * Event triggered when the write buffer was filled and is now empty.
   * Note that this does not mean that data is immediately ready to be sent
   * again -- there could still be a flow control delay.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3B07538100AE
  virtual void onDoneWriting();

protected:
  /**
   * The reliable socket.
   */
  //##ModelId=3B08920B01C2
  NLsocket rsocket;

  /**
   * The unreliable socket.
   */
  //##ModelId=3B0891DB000A
  NLsocket usocket;

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

  //Functions for PacketStream and parsing packets
  //##ModelId=3B6B302400CA
  int rawRead(bool reliable, const NLbyte* buf, int bufSize);
  //##ModelId=3B6B302401D6
  int rawWrite(bool reliable, const NLbyte* buf, int bufSize);
  friend class PacketStream;

	/**
	 * Parses the packets recieved, then calls onReceive.
	 */
  //##ModelId=3B07538100B0
  void onReceive(bool reliable);

};

}
#endif /* CONNECTION_H_INCLUDED_C51DC478 */


