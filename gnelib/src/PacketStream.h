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

namespace GNE {
class Packet;
class Connection;

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
   */
  //##ModelId=3B07538101BD
  PacketStream(int outRate2, int inRate2, Connection& ourOwner);

  //##ModelId=3B07538101C0
  virtual ~PacketStream();

  /**
   * Returns the current incoming queue length in packets.
   */
  //##ModelId=3B07538101C2
  int getInLength() const;

  /**
   * Returns the current outgoing queue length in packets.
   */
  //##ModelId=3B07538101C4
  int getOutLength() const;

  /**
   * Is there at least one packet in the incoming queue?
   */
  //##ModelId=3B07538101C6
  bool isNextPacket() const;

  /**
   * Returns the next packet from the queue, removing it from that queue.
   * It is your responsibility to deallocate the memory for this packet.
   * @return A pointer to the next packet, which you are responsible for
   *         deleting, or NULL if there is no next packet.
   */
  //##ModelId=3B07538101C8
  Packet* getNextPacket();

  /**
   * Adds a packet to the outgoing queue.
   * @param packet the packet to send.
   * @param should this packet be sent reliably if the connection supports it?
   */
  //##ModelId=3B07538101C9
  void writePacket(const Packet& packet, bool reliable);

  /**
   * Returns the actual incoming data rate, which may be the same or less
   * that what was originally requested on connection.
   */
  //##ModelId=3B07538101F5
  int getInRate() const;

  /**
   * Returns the actual outgoing data rate, which may be the same or less
   * that what was originally requested on connection.
   */
  //##ModelId=3B07538101F7
  int getOutRate() const;

  /**
   * Blocks on this PacketStream until all packets have been sent.
   */
  //##ModelId=3B07538101F9
  void waitToSendAll();

	/**
	 * Overrides Thread::shutDown so that the PacketStream daemon thread will
	 * be woken up since it might be waiting on a ConditionVariable.
	 */
	void shutDown();

  /**
   * Internal function -- Do not call.  This thread handles throttled writes
   * to the socket.
   */
  //##ModelId=3B07538101FA
  void run();

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

private:

  //##ModelId=3B6B30250015
  Connection& owner;

  //##ModelId=3AE45A8302E4
  std::queue<Packet*> in;

  //##ModelId=3B075380030E
  struct PacketStreamData {
    //##ModelId=3B0753810188
    Packet* packet;
    //##ModelId=3B075381018C
    bool    reliable;
  };
  //##ModelId=3B07538101FD
  PacketStreamData* getNextPacketToSend();

  //##ModelId=3AE45B56017C
  std::queue<PacketStreamData*> out;

  //##ModelId=3B075381018E
  int inRate;

  //##ModelId=3B075381018F
  int outRate;

  //##ModelId=3B0753810196
  mutable Mutex inQCtrl;

  //##ModelId=3B07538101B9
  mutable ConditionVariable outQCtrl;

};

}
#endif /* PACKETSTREAM_H_INCLUDED_C51CCBFF */


