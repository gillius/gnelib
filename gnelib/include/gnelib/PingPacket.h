#ifndef _PINGPACKET_H_
#define _PINGPACKET_H_

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

#include "Packet.h"
#include "Mutex.h"
#include "Time.h"
#include <map>

namespace GNE {

/**
 * Using the PingPacket is one way to measure ping.  Other methods exist to
 * find ping and latency (latency is one-way time, whereas ping is two-way).
 * You could use other methods depending on if data sent in the game is an
 * implied response (and therefore a "built-in" ping), or if the peers are
 * time-syncronized and timestamps are sent on game packets this can be used
 * to measure latency with every incoming packet.
 *
 * So PingPacket is just a very simple system of measuring ping and is handy
 * for measuring ping while connecting, or when making a simple "query"
 * connection like when a client is just trying to get game info.  Ping
 * capability was not integrated into the Connection class to give you the
 * largest flexibility on how to measure latency and ping in your games.
 *
 * Since the PingPackets will be placed into the queue and received as
 * normal, this will be measuring effective ping rather than the actual time
 * it takes for the packet to travel there and back, giving a more realistic
 * view of lag.
 *
 * The way to use PingPackets:
 * <ul>
 *   <li>Create a new PingPacket.  The constructor will make a ping request
 *       with the proper unique request ID.</li>
 *   <li>Send the packet to whoever you want, any way you want.</li>
 *   <li>When reading the incoming packet stream, check for PingPackets.<br>
 *       Discover if they are requests or replies by calling isRequest.<br>
 *       If they are requests, call makeReply and send it back.<br>
 *       If they are replies, call getPing to measure the round trip time.</li>
 *   <li>If you send packets over an unreliable connection, call
 *       recoverLostRequests to declare late packets as "lost" and recover any
 *       used memory in the request table.</li>
 * </ul>
 *
 * Check out the exping example for an example and more explaination on how
 * to use PingPacket.
 */
class PingPacket : public Packet {
public:
  /**
   * Creates a PingPacket with a preassigned request ID which is pulled from
   * an internal source.  Only ping requests are made using the constructor --
   * you should use getReply when you get a ping request to make the ping
   * response packet.
   * The PingPacket::create function passes in false to get an uninitalized
   * PingPacket for reading.  You will probably only ever use the default
   * form of the constructor.  If you pass false, the state of this object is
   * undefined after creation and is suitable only to use readPacket on.
   */
  PingPacket(bool makeReq = true);

  virtual ~PingPacket();

  /**
   * The ID for this type of packet.
   */
  static const int ID;

  /**
   * Is this PingPacket a ping request?
   */
  bool isRequest();

  /**
   * Changes this PingPacket from a ping request to a ping reply.  If the
   * PingPacket is already a reply, this function has no net effect.
   */
  void makeReply();

  /**
   * If this packet is a ping reply that is from one of our requests, this
   * finds the time elapsed since its creation.  Calling this function will
   * remove the request ID from the request table, so this function will only
   * work once for each reply.  If the request ID was not found, then an
   * elapsed time of 0 will be returned.
   */
  Time getPing();

  /**
   * Every time you create a packet, the request ID is placed into a request
   * table so that the time difference can be measured later.  This takes up
   * some memory.  It is possible that a ping reply is never received,
   * espically if sending PingPackets over an unreliable connection.  This
   * function will remove any packets that have been pending for longer than
   * the time specified.  It will return the number of packets that were
   * considered lost based on the specified pending time limit and remove
   * these requests from the table.  If a reply for one of these lost
   * requests comes back, getPing will be unable to find them and return an
   * elapsed time of 0.
   *
   * When GNE is shutdown the request table will be cleared out so you don't
   * have to ever call this function unless you want to find out how many
   * more packets have been lost, or if you want to free some memory.
   */
  static int recoverLostRequests(Time limit);

  /**
   * Returns the number of pending requests.  This number increases by 1
   * every time a request is made, and goes down by one with a successful
   * call to getPing.  The number will also decrease if recoverLostReqests
   * is called and finds late requests.
   */
  static int reqsPending();

  /**
   * Returns a newly allocated exact copy of this packet.
   */
  virtual Packet* makeClone() const;

  /**
   * Returns the current size of this packet in bytes.
   */
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket. 
   */
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.
   */
  virtual void readPacket(RawPacket& raw);

  /**
   * Returns a new instance of this class using the constructor to pass in
   * false, so this returns an object in an uninitialized state and suitable
   * only to call readPacket on.
   */
  static Packet* create();

private:
  gbool isReq;

  guint32 reqId;

  //Provides syncronization for nextReqId and requests
  static Mutex sync;

  static guint32 nextReqId;

  static std::map<guint32, Time> requests;

};

}
#endif
