#ifndef PACKET_H_INCLUDED_C51B374A
#define PACKET_H_INCLUDED_C51B374A

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

namespace GNE {
class RawPacket;

/**
 * The base packet class, used for dealing with all types of packets at a
 * fundamental level.  All types of packets must inherit from this class to
 * be recognized by GNE.  GNE already implmenets some types of packets.
 *
 * When you create a new packet you MUST define your own versions of all
 * virtual functions or the program will fail.
 *
 * See the example expacket on how to properly derive from a Packet class, or
 * look at the code for the other GNE packets.
 */
class Packet {
public:
  /**
   * Constructs a packet with the given ID.  If you pass no ID, the ID for an
   * empty packet is assumed.  Normally sending blank packets are not useful,
   * however.  If you are wanting to use a blank packet for some purpose such
   * as a end-of-data marker or for sending some message that requires no
   * data, it is suggested that you simply derive a class from Packet that
   * adds no data, but has a unique ID so it can be "recognized" easier.
   * @param id a number from PacketParser::MIN_USER_ID to 255.  GNE packet
   *        id's are from 0 to MIN_USER_ID-1, inclusive.
   */
  explicit Packet(int id = 0);

  Packet( const Packet& o );

  virtual ~Packet();

  /**
   * The ID for this type of packet.
   */
  static const int ID;

  /**
   * Returns a newly allocated exact copy of this packet.
   */
  virtual Packet* makeClone() const;

  /**
   * Returns the type of this instance.  This allows you to identify the type
   * of packet you have when you only have a Packet*.
   */
  int getType() const;

  /**
   * Returns the current size of this packet in bytes.  When overloading this
   * function, call getSize on the parent class then add the sizes of your
   * additional variables.  If the size cannot be determined, then getSize
   * should return a value <= RawPacket::RAW_PACKET_LEN but >= its possible
   * size -- so in other words if the size cannot be determined, it should
   * return the largest possible size that given packet could be.  This is
   * discouraged as much as possible since GNE allocates packets in the data
   * stream based on this value, and large values will hinder performance.
   */
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket.  You can continue writing more
   * packets to the RawPacket after this method.  You must make sure there
   * is enough space in the RawPacket to fit this new packet.  When
   * overloading this function, call writePacket on the parent class then
   * write your own variables.
   */
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.  When overloading this
   * function, call readPacket on the parent class then read your own
   * variables.
   *
   * Note that the parser has already read the packet type from raw so it can
   * create this packet, so it should not be re-read.  This object already
   * knows its type from creation.  This is only relevant to the implemenation
   * of the actual Packet class, and has no signifiance to derived classes,
   * or to the end user.
   */
  virtual void readPacket(RawPacket& raw);

  /**
   * Returns a new instance of this class.  This is used by the
   * PacketParser::registerPacket function as a callback to make new packets
   * to parse the data into.  If you derive a new packet type, you'll probably
   * want to register it, so you should create a create function of your own.
   */
  static Packet* create();

  /**
   * Copy operator you can use to help you in creating your own.
   * There is a debug assert in this function that checks to make sure the
   * types match.  Call this operator first from your copy operator.  Many
   * GNE packets may not support this operation, so check the documentation
   * first -- if no operator = exists, then assume you cannot copy packets
   * this way, unless the documentation says otherwise.<br>
   * If you can't use operator= on a packet, you can use makeClone to
   * achieve a nearly equivalent result.
   */
  Packet& operator = (const Packet& rhs);

private:
  /**
   * The type ID for this Packet.
   */
  guint8 type;
};

}
#endif /* PACKET_H_INCLUDED_C51B374A */

