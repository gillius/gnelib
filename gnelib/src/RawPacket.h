#ifndef RAWPACKET_H_INCLUDED_C51B6961
#define RAWPACKET_H_INCLUDED_C51B6961

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

/**
 * A utility class for creating a raw block of data that is used to directly
 * send data through the socket, and on the other side, parse it into
 * system-dependant data.  Users of GNE can also use this class to serialize
 * packets into a form suitable to be sent over their own types of network
 * connections or saved/loaded from a file.  A neat trick might be implemented
 * where a file can "mimic" a server to provide the necessary packets to
 * restore the game state.\n
 * See the example "expacket" for an example of the usage of this class.
 */
//##ModelId=3AE4D1C600F0
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
  //##ModelId=3AE4D3B30320
  RawPacket(NLbyte* buffer = NULL);

  /**
   * Destroys this object.  The data buffer will be destroyed at this point,
   * so you cannot use any pointers you got back from getData() after this
   * point.
   */
  //##ModelId=3AE4D3BD008C
  virtual ~RawPacket();

  /**
   * Gets the raw block of data.  This will be used by the library to get the
   * data that will actually be sent out to the socket.
   */
  //##ModelId=3AE4D3F303C0
  const NLbyte* getData() const;

  /**
   * Returns the length of the internal buffer in bytes.  This buffer is what
   * you get from getData().
   */
  //##ModelId=3B05F5980118
  int getLength() const;

  /**
   * Resets the reading/writing position back to 0.  When writing, this
   * essentially "erases" all data, and when reading this starts reading
   * from the beginning again.
   */
  //##ModelId=3AE4D67700F0
  void reset();

  /**
   * Writes raw data starting at the current position.  This acts just like
   * the memcpy command.
   */
  //##ModelId=3B05F5980187
  void writeRaw(const NLbyte* block, int length);

  /**
   * Like writeBlock, but just the other way around now :).
   * @see writeBlock
   */
  //##ModelId=3B05F5980370
  void readRaw(NLbyte* block, int length);

  /**
   * Stream operators for writing to this RawPacket.  All data is converted
   * when appropriate into big endian format.
   */
  //##ModelId=3B05F59901AE
  RawPacket& operator << (signed char x);
  //##ModelId=3B05F59902BC
  RawPacket& operator << (NLbyte x);
  //##ModelId=3B05F59A001E
  RawPacket& operator << (NLshort x);
  //##ModelId=3B05F59A0136
  RawPacket& operator << (NLushort x);
  //##ModelId=3B05F59A0280
  RawPacket& operator << (NLint x);
  //##ModelId=3B05F59A03CA
  RawPacket& operator << (NLuint x);
  //##ModelId=3B05F59B00F0
  RawPacket& operator << (NLfloat x);
  //##ModelId=3B05F59B023A
  RawPacket& operator << (NLdouble x);
  //##ModelId=3B05F59B0384
  RawPacket& operator << (const std::string& x);

  /**
   * Stream operators for reading from this RawPacket.  All data is converted
   * when appropriate from big endian format.
   */
  //##ModelId=3B05F59C00E6
  RawPacket& operator >> (signed char& x);
  //##ModelId=3B05F59C0230
  RawPacket& operator >> (NLbyte& x);
  //##ModelId=3B05F59C037A
  RawPacket& operator >> (NLshort& x);
  //##ModelId=3B05F59D00DC
  RawPacket& operator >> (NLushort& x);
  //##ModelId=3B05F59D0226
  RawPacket& operator >> (NLint& x);
  //##ModelId=3B05F59D03A2
  RawPacket& operator >> (NLuint& x);
  //##ModelId=3B05F59E0104
  RawPacket& operator >> (NLfloat& x);
  //##ModelId=3B05F59E024E
  RawPacket& operator >> (NLdouble& x);
  //##ModelId=3B05F59F00C8
  RawPacket& operator >> (std::string& x);

  /**
   * The length of a RawPacket.  The internal buffer for a RawPacket used for
   * writing will always have a buffer of this size.
   */
  //##ModelId=3B05F597030C
  static const int RAW_PACKET_LEN;

private:
  //##ModelId=3B05F597037A
  bool ourBuffer;

  //##ModelId=3AE4D1E50226
  int length;

  //##ModelId=3AE4D1ED0014
  NLbyte* data;

  /**
   * The current location of the next data read/write.
   */
  //##ModelId=3AE4D40A035C
  int currLoc;

};

#endif /* RAWPACKET_H_INCLUDED_C51B6961 */