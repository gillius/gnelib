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
#include "../include/gnelib/RateAdjustPacket.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

//##ModelId=3C783ACF02A1
const int RateAdjustPacket::ID = 3;

//##ModelId=3C783ACF02AA
RateAdjustPacket::RateAdjustPacket() : Packet(ID), rate(0) {
}

//##ModelId=3C783ACF02AB
RateAdjustPacket::~RateAdjustPacket() {
}

//##ModelId=3C783ACF02AD
Packet* RateAdjustPacket::makeClone() const {
  return new RateAdjustPacket(*this);
}

//##ModelId=3C783ACF02AF
int RateAdjustPacket::getSize() const {
  return Packet::getSize() + sizeof(rate);
}

//##ModelId=3C783ACF02B1
void RateAdjustPacket::writePacket(RawPacket& raw) const {
  Packet::writePacket(raw);
  raw << rate;
}

//##ModelId=3C783ACF02B4
void RateAdjustPacket::readPacket(RawPacket& raw) {
  Packet::readPacket(raw);
  raw >> rate;
}

//##ModelId=3C783ACF02B7
Packet* RateAdjustPacket::create() {
  return new RateAdjustPacket;
}

}
