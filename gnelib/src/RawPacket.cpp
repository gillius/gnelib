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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Packet.h"

namespace GNE {

const int RawPacket::RAW_PACKET_LEN = 512;

//##ModelId=3B075381022A
RawPacket::RawPacket(gbyte* buffer) : currLoc(0) {
  if (buffer) {
    data = buffer;
    ourBuffer = false;
  } else {
    data = new gbyte[RAW_PACKET_LEN + 1]; //last byte for END_OF_PACKET
    ourBuffer = true;
  }
}

//##ModelId=3B075381022C
RawPacket::~RawPacket() {
  if (ourBuffer)
    delete[] data;
}

//##ModelId=3B075381022E
const gbyte* RawPacket::getData() const {
  return data;
}

//##ModelId=3B0753810230
int RawPacket::getPosition() const {
  return currLoc;
}

//##ModelId=3B0753810232
void RawPacket::reset() {
  currLoc = 0;
}

//##ModelId=3B0753810233
void RawPacket::writeRaw(const gbyte* block, int length) {
  writeBlock(data, currLoc, block, length);
  assert(currLoc <= RAW_PACKET_LEN);
}

//##ModelId=3B0753810236
void RawPacket::readRaw(gbyte* block, int length) {
  readBlock(data, currLoc, block, length);
  assert(currLoc <= RAW_PACKET_LEN);
}

//START OF WRITING OPERATORS

//##ModelId=3B0753810239
RawPacket& RawPacket::operator << (gint8 x) {
  writeByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023B
RawPacket& RawPacket::operator << (guint8 x) {
  writeByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023D
RawPacket& RawPacket::operator << (gint16 x) {
  writeShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023F
RawPacket& RawPacket::operator << (guint16 x) {
  writeShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810241
RawPacket& RawPacket::operator << (gint32 x) {
  writeLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810243
RawPacket& RawPacket::operator << (guint32 x) {
  writeLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810245
RawPacket& RawPacket::operator << (gsingle x) {
  writeFloat(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810262
RawPacket& RawPacket::operator << (gdouble x) {
  writeDouble(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810264
RawPacket& RawPacket::operator << (const std::string& x) {
  assert(x.size() <= 255);
  guint8 length = (guint8)x.size();

  //Write the length byte
  *this << length;

  if (length) {
    //If the string contains data, copy it raw into the data buffer
    gbyte* start = &data[currLoc];
    memcpy((void*)start, (const void*)x.data(), length);
    currLoc += (int)length;
  }

  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0C2CE50226
RawPacket& RawPacket::operator << (const Packet& x) {
  x.writePacket(*this);
  return *this;
}

//START OF READING OPERATORS

RawPacket& RawPacket::operator >> (gint8& x) {
  readByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (guint8& x) {
  readByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (gint16& x) {
  readShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (guint16& x) {
  readShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (gint32& x) {
  readLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (guint32& x) {
  readLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (gsingle& x) {
  readFloat(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (gdouble& x) {
  readDouble(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (std::string& x) {
  //Get the length byte of the string.
  guint8 length;
  *this >> length;

  if (length) {
    //If the string has a non-zero length, create a new string.
    std::string temp((char*)&data[currLoc], (int)length);
    x = temp;
    currLoc += (int)length;
  }

  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (Packet& x) {
  x.readPacket(*this);
  return *this;
}

}
