#ifndef RAWPACKET_H_INCLUDED_C51B6961
#define RAWPACKET_H_INCLUDED_C51B6961

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

#include "gnetypes.h"

namespace GNE {
class Packet;
class Time;

/**
 * @ingroup midlevel
 *
 * A utility class for creating a raw block of data that is used to directly
 * send data through the socket, and on the other side, parse it into
 * system-dependant data.  Users of GNE can also use this class to serialize
 * packets into a form suitable to be sent over their own types of network
 * connections or saved/loaded from a file.  A neat trick might be implemented
 * where a file can "mimic" a server to provide the necessary packets to
 * restore the game state.
 *
 * The name of this class has confused a few.  RawPacket is not derived from
 * GNE::Packet, so it is not an actual GNE packet.  This class represents the
 * raw block of memory that will be sent over the network.  There is a class
 * called CustomPacket which will allow you to send RawPackets directly
 * across the network.
 *
 * See the example "expacket" for an example of the usage of this class.
 */
class RawPacket {
public:
  /**
   * Create a new RawPacket, optionally with a buffer.  If you pass NULL to
   * this constructor, a new internal buffer will be created, and this
   * RawPacket will then be meant for writing to only.  If you pass a buffer
   * to this constructor, this object will be meant from reading from only.
   * If you pass a buffer into RawPacket, you should delete it yourself later.
   * @param buffer NULL for a writable object, or your own buffer for a
   *               readable object, which you will destroy after this object
   *               has been destroyed.
   */
  RawPacket(gbyte* buffer = NULL);

  /**
   * Destroys this object.  The data buffer will be destroyed at this point,
   * so you cannot use any pointers you got back from getData() after this
   * point.
   */
  virtual ~RawPacket();

  /**
   * Gets the raw block of data.  This will be used by the library to get the
   * data that will actually be sent out to the socket.
   */
  const gbyte* getData() const;

  /**
   * Returns the position in the buffer from 0.  If you are writing to this
   * RawPacket, this will give the next writing position -- which will also
   * be the length of the data written thus far.  If you are reading, this
   * will be the position of the next byte to be read -- the amount of data
   * read thus far.  This way you can tell how much was written or how much
   * was read back in bytes.
   */
  int getPosition() const;

  /**
   * Resets the reading/writing position back to 0.  When writing, this
   * essentially "erases" all data, and when reading this starts reading
   * from the beginning again.
   */
  void reset();

  /**
   * Returns the serialized size of the given string.  This is meant for use
   * in your overridden Packet::getSize method to find the size of strings.
   * For all other primitives, use getSizeOf(variable).
   */
  static int getStringSize(const std::string& x) { return (int)(x.size() + 1); }

  /**
   * Returns the maximum possible serialized size of a string of the given
   * length in ASCII characters.
   */
  static int getStringSize(int x) { return x + 1; }

  /**
   * Returns the serialized size in bytes of the given variable, to be used
   * in your overridden Packet::getSize method.
   */
  static int getSizeOf(const std::string& x) { return getStringSize( x ); }
  static int getSizeOf(guint8 x) { return sizeof(x); }
  static int getSizeOf(gint16 x) { return sizeof(x); }
  static int getSizeOf(guint16 x) { return sizeof(x); }
  static int getSizeOf(gint32 x) { return sizeof(x); }
  static int getSizeOf(guint32 x) { return sizeof(x); }
  static int getSizeOf(gsingle x) { return sizeof(x); }
  static int getSizeOf(gdouble x) { return sizeof(x); }
  static int getSizeOf(const Time& x) { return sizeof(gint32)*2; }

  /**
   * Writes raw data starting at the current position.  This acts similar to
   * the memcpy function.
   */
  void writeRaw(const gbyte* block, int length);

  /**
   * Like writeRaw, but just the other way around now :).
   * @see writeRaw
   */
  void readRaw(gbyte* block, int length);

  /**
   * Stream operators for writing to this RawPacket.  All data is converted
   * when appropriate into little endian format.
   */
  RawPacket& operator << (gint8 x);
  /**
   * This operator also works for gbool and gbyte.
   */
  RawPacket& operator << (guint8 x);
  RawPacket& operator << (gint16 x);
  RawPacket& operator << (guint16 x);
  RawPacket& operator << (gint32 x);
  RawPacket& operator << (guint32 x);
  RawPacket& operator << (gsingle x);
  RawPacket& operator << (gdouble x);
  RawPacket& operator << (const std::string& x);
  RawPacket& operator << (const Time& x);

  /**
   * Writes a packet to the RawPacket.  This function will simply call the
   * packet's writePacket function.
   * @see Packet::writePacket
   */
  RawPacket& operator << (const Packet& packet);

  /**
   * Stream operators for reading from this RawPacket.  All data is converted
   * when appropriate from big endian format.
   */
  RawPacket& operator >> (gint8& x);
  /**
   * This operator also works for gbool and gbyte.
   */
  RawPacket& operator >> (guint8& x);
  RawPacket& operator >> (gint16& x);
  RawPacket& operator >> (guint16& x);
  RawPacket& operator >> (gint32& x);
  RawPacket& operator >> (guint32& x);
  RawPacket& operator >> (gsingle& x);
  RawPacket& operator >> (gdouble& x);
  RawPacket& operator >> (std::string& x);
  RawPacket& operator >> (Time& x);

  /**
   * This function calls the packet's readPacket function.  You will already
   * need to know what kind of packet is next.  If you don't, use
   * PacketPaser::parseNextPacket.
   * @see PacketParser::parseNextPacket
   * @see Packet::readPacket
   */
  RawPacket& operator >> (Packet& packet);

  /**
   * The max length of a RawPacket.  The internal buffer for a RawPacket
   * used for writing will always have a buffer of this size.
   */
  static const int RAW_PACKET_LEN;

private:
  bool ourBuffer;

  gbyte* data;

  /**
   * The current location of the next data read/write.
   */
  int currLoc;

};

}
#endif /* RAWPACKET_H_INCLUDED_C51B6961 */
