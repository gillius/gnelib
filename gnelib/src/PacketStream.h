#ifndef PACKETSTREAM_H_INCLUDED_C51CCBFF
#define PACKETSTREAM_H_INCLUDED_C51CCBFF

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
#include "ConditionVariable.h"
#include "ConnectionWriter.h"
#include "Mutex.h"
class Packet;

/**
 * This class resembles a packet stream through a connection.
 */
//##ModelId=3AE35F9E001E
class PacketStream {
public:
  //##ModelId=3AE3605F032A
  PacketStream();

  //##ModelId=3AE36060001E
  virtual ~PacketStream();

  /**
   * Returns the current incoming queue length in packets.
   */
  //##ModelId=3AE450930172
  int getInLength() const;

  /**
   * Returns the current outgoing queue length in packets.
   */
  //##ModelId=3AE450AD019A
  int getOutLength() const;

  /**
   * Is there at least one packet in the incoming queue?
   */
  //##ModelId=3AE44F5B0078
  bool isNextPacket() const;

  /**
   * Returns the next packet from the queue, removing it from that queue.
   * It is your responsibility to deallocate the memory for this packet.
   */
  //##ModelId=3AE4503000B4
  Packet* getNextPacket();

  /**
   * Adds a packet to the outgoing queue.
   * @param packet the packet to send.
   * @param should this packet be sent reliably if the connection supports it?
   */
  //##ModelId=3AE4503E023A
  void writePacket(const Packet& packet, bool reliable);

  /**
   * Internal function used by ConnectionWriter -- do not use.  Removes the
   * next packet from the outgoing queue.
   */
  //##ModelId=3AE4CDD70122
  Packet* getNextPacketToSend();

  /**
   * Returns the actual incoming data rate, which may be the same or less
   * that what was originally requested on connection.
   */
  //##ModelId=3AE4C60C026C
  int getInRate() const;

  /**
   * Returns the actual outgoing data rate, which may be the same or less
   * that what was originally requested on connection.
   */
  //##ModelId=3AE4C60C0316
  int getOutRate() const;

  /**
   * Blocks on this PacketStream until all packets have been sent.
   */
  //##ModelId=3AE4C70203C0
  void waitToSendAll();

private:

  //##ModelId=3AE45A8302E4
  std::queue<Packet*> in;

  //##ModelId=3AE45C4303DE
  struct PacketStreamData {
    //##ModelId=3AE45C440066
    Packet* packet;
    //##ModelId=3AE45C44006A
    bool    reliable;
  };

  //##ModelId=3AE45B56017C
  std::queue<PacketStreamData*> out;

  //##ModelId=3AE4C5D30226
  int inRate;

  //##ModelId=3AE4C5F7038E
  int outRate;

  //##ModelId=3AE4C45D0046
  ConditionVariable writeSync;

  //##ModelId=3AE4CD6E0370
  ConnectionWriter writer;

  //##ModelId=3AE4CED502C6
  Mutex inQCtrl;

  //##ModelId=3AE4CF72001E
  Mutex outQCtrl;

};



#endif /* PACKETSTREAM_H_INCLUDED_C51CCBFF */


