#ifndef PACKETSTREAM_H_INCLUDED_C51CCBFF
#define PACKETSTREAM_H_INCLUDED_C51CCBFF

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
#include "ConditionVariable.h"
#include "Mutex.h"
#include "Thread.h"
#include "Time.h"

namespace GNE {
class Packet;
class Connection;
class RawPacket;

/**
 * This class resembles a packet stream through a connection.  This class
 * is maintained by the Connection class, and you shouldn't have to create
 * your own PacketStreams.
 * NOTE: all functions in this class are thread safe, since this class uses
 *       its own mutexes internally.  Note that data in the class may change
 *       between calls, if another thread changes its state.
 */
//##ModelId=3B075380030D
class PacketStream : public Thread {
public:
  /**
   * Creates a new PacketStream with the given flow control parameters.
   * Passing a value 0 for a rate is interpreted as "unlimited" or
   * unrestricted rates.  Passing a value less than 0 is not allowed.
   * @param reqOutRate2 This is the out rate that we are requesting, or in
   *                    other words, the maximum rate we are willing to send.
   * @param maxOutRate2 The maximum rate the remote machine is letting us
   *                    send.  The actual outgoing rate, therefore, is the
   *                    minimum of the two outgoing rate values.
   */
  //##ModelId=3B07538101BD
  PacketStream(int reqOutRate2, int maxOutRate2, Connection& ourOwner);

  /**
   * Destroys this object.  Any data left remaining in the in or out queues
   * is destroyed as well.
   */
  //##ModelId=3B07538101C0
  virtual ~PacketStream();

  /**
   * Returns the current incoming queue length in packets.
   */
  //##ModelId=3B07538101C2
  int getInLength() const;

  /**
   * Returns the current outgoing queue length in packets.  This is meant
   * as a possible hint for your application to tune its performance by
   * knowning when the queues are starting to fill up.
   *
   * @param reliable true for the outgoing reliable packet queue.<br>
   *                 false for the incoming reliable packet queue.
   */
  //##ModelId=3B07538101C4
  int getOutLength(bool reliable) const;

  /**
   * Is there at least one packet in the incoming queue?  Note that this does
   * not guarantee that getNextPacket will return a non-NULL value if it is
   * possible for any other thread to try to get the data between your call
   * and getNextPacket.  This is only useful if you want to passively check
   * for incoming data.  If you are wanting to do anything with that data it
   * is suggested that you use getNextPacket and if it is non-NULL then
   * process the data.  That method is thread safe.
   */
  //##ModelId=3B07538101C6
  bool isNextPacket() const;

  /**
   * Returns the next packet from the queue, removing it from that queue.
   * It is your responsibility to deallocate the memory for this packet as
   * the calling code becomes the owner of the memory the returned packet
   * occupies.
   * @return A pointer to the next packet, which you are responsible for
   *         deleting, or NULL if there is no next packet.
   */
  //##ModelId=3B07538101C8
  Packet* getNextPacket();

  /**
   * Adds a packet to the outgoing queue.  The packet given will be copied.
   * @param packet the packet to send.
   * @param should this packet be sent reliably if the connection supports it?
   */
  //##ModelId=3B07538101C9
  void writePacket(const Packet& packet, bool reliable);

  /**
   * Returns the actual outgoing data rate, which may be the same or less
   * that what was originally requested on connection.  This value is the
   * minimum between the max rate limit from the remote computer, and our
   * maximum outgoing rate.  A value of 0 means there is no outgoing rate
   * limit.
   *
   * If the requested rate changes, or if the remote computer changes its
   * max allowed limit, this number will change to the new minimum between
   * these rates.
   */
  //##ModelId=3B07538101F7
  int getCurrOutRate() const;

  /**
   * Returns the maximum outgoing rate that we are requesting.  The value 0
   * means that no rate limit was requested.
   * @see getCurrOutRate
   * @see setRates
   */
  //##ModelId=3C7AB4C501C1
  int getReqOutRate() const;

  /**
   * Returns the maximum outgoing rate that the remote computer is allowing
   * us to send to it.  This value can change if the remote machine changes
   * its rates through the setRates function.  The value 0 means that no
   * limit was requested.
   * @see getCurrOutRate
   * @see setRates
   */
  //##ModelId=3C7AB4C501CB
  int getRemoteOutLimit() const;

  /**
   * Sets new values that we are willing to send or receive.  See the
   * constructor for more information.  Pass a value less than 0 to leave one
   * of the rates unchanged.  Pass the value 0 for "unrestricted" rates.
   * Changing the rates might cause a packet to get added to the outgoing
   * packet stream to communicate this change to the other side.
   *
   * @see PacketStream::PacketStream
   */
  //##ModelId=3C783ACF0264
  void setRates(int reqOutRate2, int maxInRate2);

  /**
   * Blocks on this PacketStream until all packets have been sent.
   * @param waitTime the max amount of time in ms to wait for the outgoing
   *        packet queue to clear.
   */
  //##ModelId=3B07538101F9
  void waitToSendAll(int waitTime = 10000) const;

  /**
   * Overrides Thread::shutDown so that the PacketStream daemon thread will
   * be woken up since it might be waiting on a ConditionVariable.
   */
  //##ModelId=3B8DC5D10096
  void shutDown();

  /**
   * Add the given packet to the incoming queue.  This is normalled used
   * internally by the Connection class to add the packets, but it is safe
   * for the user to call, if they want to delay processing of the packets
   * for a later time and see what other packets are available.\n
   * Remember queues are LIFO, meaning this inserted packet will be the last
   * in the queue after addition.\n
   * Also note that when you pass this object into the queue, you will
   * eventually get it back from getNextPacket(), so you will want to handle
   * deallocation at that point.
   */
  //##ModelId=3B07538101FB
  void addIncomingPacket(Packet* packet);

protected:
  /**
   * This thread handles throttled writes to the socket.
   */
  //##ModelId=3B07538101FA
  void run();

private:

  //##ModelId=3C7867230185
  void prepareSend(std::queue<Packet*>& q, RawPacket& raw);

  //##ModelId=3B6B30250015
  Connection& owner;

  //##ModelId=3AE45A8302E4
  std::queue<Packet*> in;

  //##ModelId=3AE45B56017C
  std::queue<Packet*> outUnrel;

  std::queue<Packet*> outRel;

  //##ModelId=3B075381018F
  int maxOutRate;

  //##ModelId=3C783ACF021E
  int reqOutRate;

  //This is the precalculated min of maxOutRate and reqOutRate.
  //##ModelId=3C783ACF0232
  int currOutRate;

  /**
   * This is the current number of bytes we are allowed to send without
   * waiting.  This value should increase per the outRateStep until it
   * reaches a maximum value of currOutRate.
   */
  //##ModelId=3C783ACF023C
  int outRemain;

  /**
   * This is the "step" that the out value increases for every step in
   * time.  The size of the step is defined by the code.
   */
  //##ModelId=3C783ACF0246
  int outRateStep;

  /**
   * The last time the rate was calculated.
   */
  //##ModelId=3C783ACF025B
  Time lastTime;

  /**
   * Calculates the current rate and step based on the current values for
   * maxOutRate and reqOutRate.
   */
  //##ModelId=3C7960970177
  void setupCurrRate();

  /**
   * Discovers the amount of time passed and updates the currOutRemain so we
   * know much data we can send.  This should be called almost everytime
   * before we use currOutRemain.  outQCtrl MUST be acquired when you call
   * this function.
   */
  //##ModelId=3C783ACF028C
  void updateRates();

  /**
   * These are set to be mutable because of the const functions need
   * non-const access to these objects, but they can still be called const
   * because the object's state is the same before and after the method.
   */
  //##ModelId=3B0753810196
  mutable Mutex inQCtrl;

  //##ModelId=3B07538101B9
  mutable ConditionVariable outQCtrl;

};

}
#endif /* PACKETSTREAM_H_INCLUDED_C51CCBFF */


