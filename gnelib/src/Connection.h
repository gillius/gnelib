#ifndef CONNECTION_H_INCLUDED_C51DC478
#define CONNECTION_H_INCLUDED_C51DC478

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
#include "Thread.h"
#include "PacketStream.h"

/**
 * A class resembling any type of connection to a remote computer.  A
 * connection also manages flow control.  When a new Connection instance is
 * constructed the passed inRate and outRate parameters set a maximum
 * bandwith usage for either direction.  These values might stay the same or
 * reduce after a connection to the remote machine is made, based on that
 * machine's requested maximums.
 */
//##ModelId=3AE2167E01EA
class Connection {
public:
  /**
   * A struct holding values returned by getStats.  All values are
   * in actual bytes per second, not program data bytes (meaning this
   * includes the bytes sent in UDP, TCP, and IP packet headers.
   */
  //##ModelId=3AE4CABD00F0
  struct Stats {
    //##ModelId=3AE4CB9302D0
    int sent;
    //##ModelId=3AE4CBBF03AC
    int recv;
    //##ModelId=3AE4CBC403DE
    int maxAvgSent;
    //##ModelId=3AE4CBD10046
    int maxAvgRecv;
    //##ModelId=3AE4CBD601CC
    int avgSent;
    //##ModelId=3AE4CBDC0000
    int avgRecv;
  };

  /**
   * An enum describing connection failure type.
   */
  //##ModelId=3AE4F24B0276
  enum FailureType {
    GNEHostVersionLow = 0,
    GNEHostVersionHigh = 1,
    UserHostVersionLow = 2,
    UserHostVersionHigh = 3,
    HostDidNotRespond = 4,
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
   * @param address the target address to connect to, as a hostname or in
   *        xxx.xxx.xxx.xxx:ppppp format
   */
  //##ModelId=3AE3591A0186
  Connection(int outRate, int inRate, std::string address);

  //##ModelId=3AE3591A0187
  virtual ~Connection();

  /**
   * Returns the PacketStream associated with this class.  Writing and
   * reading through this Connection is done through this PacketStream.
   */
  //##ModelId=3AE4A6F00280
  PacketStream& stream();

  /**
   * If stats is enabled, returns Connection stats.
   * @return various connection stats.
   * @see ConnectionStats
   */
  //##ModelId=3AE4C9DD0280
  Stats getStats() const;

  /**
   * Returns true if this Connection is active and ready to send/recieve.
   */
  //##ModelId=3AE4A9820366
  bool isConnected() const;

  /**
   * Immediately disconnects this socket.  No more data will be recieved or
   * sent on this socket.  If you want to disconnect more nicely, use
   * disconnectSendAll.
   * @see #disconnectSendAll()
   */
  //##ModelId=3AE4A9700212
  void disconnect();

  /**
   * A blocking call that disconnects nicely.  This has the same effect as
   * disconnect, except that all data waiting to be sent will be sent if
   * possible.
   * @see #disconnect()
   */
  //##ModelId=3AE4C7FC021C
  void disconnectSendAll();

  /**
   * This is called when something bad happens, such as a timeout.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3AE4CD3D00E6
  virtual void onFailure(FailureType errorType);

  /**
   * Event triggered when one or more packets have been recieved.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3AE4A776033E
  virtual void onRecieve();

  /**
   * Event triggered when the write buffer was filled and is now empty.
   * Note that this does not mean that data is immediately ready to be sent
   * again -- there could still be a flow control delay.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3AE4C8F501CC
  virtual void onDoneWriting();

private:

  //##ModelId=3AE44E3302DA
  PacketStream ps;

  //##ModelId=3AE4A97A038E
  bool connected;

  //##ModelId=3AE4E2340334
  NLsocket socket;
};

#endif /* CONNECTION_H_INCLUDED_C51DC478 */


