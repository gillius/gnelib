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

const int CustomPacket::ID = 1;

CustomPacket::CustomPacket()
  : Packet(ID), data(NULL), ourBuf(NULL), contentLen(-1) {
}

CustomPacket::~CustomPacket() {
  destroyData();
}

RawPacket& CustomPacket::getData() {
  if (data == NULL) {
    //create a new RawPacket for writing
    data = new RawPacket();
  }
  //Once we let the user access the data, we are unsure of its length.
  contentLen = -1;
  return *data;
}

void CustomPacket::reset() {
  destroyData();
}

Packet* CustomPacket::makeClone() const {
  assert(data != NULL);
  assert(data->getPosition() > 0 && data->getPosition() <= getMaxUserDataSize());

  CustomPacket* ret = new CustomPacket;
  ret->getData().writeRaw(data->getData(), data->getPosition());
  return ret;
}

int CustomPacket::getSize() const {
  return Packet::getSize() + sizeof(guint16) + data->getPosition();
}

int CustomPacket::getMaxUserDataSize() {
  Packet packet;
  return RawPacket::RAW_PACKET_LEN - packet.getSize() - sizeof(guint16);
}

void CustomPacket::writePacket(RawPacket& raw) const {
  assert(data != NULL);
  //We might have a cached contentLen which we kept only to be 
  int pos = (contentLen >= 0) ? contentLen : data->getPosition();
  assert(pos > 0 && pos <= getMaxUserDataSize());

  Packet::writePacket(raw);
  raw << (guint16)pos;
  raw.writeRaw(data->getData(), pos);
}

void CustomPacket::readPacket(RawPacket& raw) {
  Packet::readPacket(raw);
  destroyData();
  
  guint16 temp;
  raw >> temp;
  contentLen = (int)temp;

  ourBuf = new gbyte[contentLen];
  raw.readRaw(ourBuf, contentLen);
  data = new RawPacket(ourBuf);
}

Packet* CustomPacket::create() {
  return new CustomPacket;
}

void CustomPacket::destroyData() {
  delete[] ourBuf;
  delete data;
  ourBuf = NULL;
  data = NULL;
  contentLen = -1;
}

} //namespace GNE
