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

/**
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
//##ModelId=3B075380030F
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
  //##ModelId=3B075381022A
  RawPacket(gbyte* buffer = NULL);

  /**
   * Destroys this object.  The data buffer will be destroyed at this point,
   * so you cannot use any pointers you got back from getData() after this
   * point.
   */
  //##ModelId=3B075381022C
  virtual ~RawPacket();

  /**
   * Gets the raw block of data.  This will be used by the library to get the
   * data that will actually be sent out to the socket.
   */
  //##ModelId=3B075381022E
  const gbyte* getData() const;

  /**
   * Returns the position in the buffer from 0.  If you are writing to this
   * RawPacket, this will give the next writing position -- which will also
   * be the length of the data written thus far.  If you are reading, this
   * will be the position of the next byte to be read -- the amount of data
   * read thus far.  This way you can tell how much was written or how much
   * was read back in bytes.
   */
  //##ModelId=3B0753810230
  int getPosition() const;

  /**
   * Resets the reading/writing position back to 0.  When writing, this
   * essentially "erases" all data, and when reading this starts reading
   * from the beginning again.
   */
  //##ModelId=3B0753810232
  void reset();

  /**
   * Returns the serialized size of the given string.  This is meant for use
   * in your overridden Packet::getSize method to find the size of strings.
   * For all other primitives, use sizeof(variable).
   */
  //##ModelId=3CE60C49010A
  static int getStringSize(std::string x) { return (int)(x.size() + 1); }

  /**
   * Returns the maximum possible serialized size of a string of the given
   * length in ASCII characters.
   */
  //##ModelId=3CE60C49011E
  static int getStringSize(int x) { return x + 1; }

  /**
   * Writes raw data starting at the current position.  This acts similar to
   * the memcpy function.
   */
  //##ModelId=3B0753810233
  void writeRaw(const gbyte* block, int length);

  /**
   * Like writeRaw, but just the other way around now :).
   * @see writeRaw
   */
  //##ModelId=3B0753810236
  void readRaw(gbyte* block, int length);

  /**
   * Stream operators for writing to this RawPacket.  All data is converted
   * when appropriate into little endian format.
   */
  //##ModelId=3B0753810239
  RawPacket& operator << (gint8 x);
  /**
   * This operator also works for gbool and gbyte.
   */
  //##ModelId=3B075381023B
  RawPacket& operator << (guint8 x);
  //##ModelId=3B075381023D
  RawPacket& operator << (gint16 x);
  //##ModelId=3B075381023F
  RawPacket& operator << (guint16 x);
  //##ModelId=3B0753810241
  RawPacket& operator << (gint32 x);
  //##ModelId=3B0753810243
  RawPacket& operator << (guint32 x);
  //##ModelId=3B0753810245
  RawPacket& operator << (gsingle x);
  //##ModelId=3B0753810262
  RawPacket& operator << (gdouble x);
  //##ModelId=3B0753810264
  RawPacket& operator << (const std::string& x);

  /**
   * Writes a packet to the RawPacket.  This function will simply call the
   * packet's writePacket function.
   * @see Packet::writePacket
   */
  //##ModelId=3B0C2CE50226
  RawPacket& operator << (const Packet& packet);

  /**
   * Stream operators for reading from this RawPacket.  All data is converted
   * when appropriate from big endian format.
   */
  //##ModelId=3B0753810266
  RawPacket& operator >> (gint8& x);
  /**
   * This operator also works for gbool and gbyte.
   */
  //##ModelId=3B0753810268
  RawPacket& operator >> (guint8& x);
  //##ModelId=3B075381026A
  RawPacket& operator >> (gint16& x);
  //##ModelId=3B075381026C
  RawPacket& operator >> (guint16& x);
  //##ModelId=3B075381026E
  RawPacket& operator >> (gint32& x);
  //##ModelId=3B0753810270
  RawPacket& operator >> (guint32& x);
  //##ModelId=3B0753810272
  RawPacket& operator >> (gsingle& x);
  //##ModelId=3B0753810274
  RawPacket& operator >> (gdouble& x);
  //##ModelId=3B0753810276
  RawPacket& operator >> (std::string& x);

  /**
   * This function calls the packet's readPacket function.  You will already
   * need to know what kind of packet is next.  If you don't, use
   * PacketPaser::parseNextPacket.
   * @see PacketParser::parseNextPacket
   * @see Packet::readPacket
   */
  //##ModelId=3B0C2CE503A2
  RawPacket& operator >> (Packet& packet);

  /**
   * The max length of a RawPacket.  The internal buffer for a RawPacket
   * used for writing will always have a buffer of this size.
   */
  //##ModelId=3B0753810226
  static const int RAW_PACKET_LEN;

private:
  //##ModelId=3B0753810227
  bool ourBuffer;

  //##ModelId=3C65C7A2028B
  gbyte* data;

  /**
   * The current location of the next data read/write.
   */
  //##ModelId=3B0753810229
  int currLoc;

};

}
#endif /* RAWPACKET_H_INCLUDED_C51B6961 */
