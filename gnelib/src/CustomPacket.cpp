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
#include "../include/gnelib/CustomPacket.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Packet.h"

namespace GNE {

//##ModelId=3C0B257E03C4
CustomPacket::CustomPacket() : Packet(1), data(NULL), ourBuf(NULL) {}

//##ModelId=3C0B257E03C5
CustomPacket::~CustomPacket() {
  destroyData();
}

//##ModelId=3C0B257E03C7
RawPacket& CustomPacket::getData() {
  if (data == NULL) {
    //create a new RawPacket for writing
    data = new RawPacket();
  }
  return *data;
}

//##ModelId=3C0B257E03C8
Packet* CustomPacket::makeClone() const {
  assert (data != NULL);
  CustomPacket* ret = new CustomPacket;
  ret->getData().writeRaw(data->getData(), data->getPosition());
  return ret;
}

//##ModelId=3C0B257E03CA
int CustomPacket::getSize() const {
  return Packet::getSize() + sizeof(NLshort) + data->getPosition();
}

//##ModelId=3C30EFBD0226
int CustomPacket::getMaxUserDataSize() {
  Packet packet;
  return RawPacket::RAW_PACKET_LEN - packet.getSize() - sizeof(NLshort);
}

//##ModelId=3C0B257E03CC
void CustomPacket::writePacket(RawPacket& raw) const {
  assert (data != NULL);
  Packet::writePacket(raw);
  raw << data->getPosition();
  raw.writeRaw(data->getData(), data->getPosition());
}

//##ModelId=3C0B257E03CF
void CustomPacket::readPacket(RawPacket& raw) {
  Packet::readPacket(raw);
  destroyData();
  
  int contentLen;
  raw >> contentLen;

  ourBuf = new NLbyte[contentLen];
  raw.readRaw(ourBuf, contentLen);
  data = new RawPacket(ourBuf);
}

//##ModelId=3C0B257E03D2
Packet* CustomPacket::create() {
  return new CustomPacket;
}

//##ModelId=3C0B257E03D4
void CustomPacket::destroyData() {
  delete[] ourBuf;
  delete data;
  ourBuf = NULL;
  data = NULL;
}

} //namespace GNE
