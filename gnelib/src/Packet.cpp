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
#include "Packet.h"
#include "RawPacket.h"

//##ModelId=3AE4D49C0168
Packet::Packet(int id) : type((NLubyte)id) {
}

//##ModelId=3AE4D49C019A
Packet::~Packet() {
}

//##ModelId=3B0723A6001E
Packet* Packet::makeClone() const {
  return new Packet(*this);
}

//##ModelId=3AE4DA920276
int Packet::getType() const {
  return (int)type;
}

//##ModelId=3B0723A600FA
int Packet::getSize() const {
  return sizeof(timestamp) + sizeof(type);
}

//##ModelId=3B06EDF30302
void Packet::writePacket(RawPacket& raw) const {
  raw << type << timestamp;
}

//##ModelId=3B06EE000352
void Packet::readPacket(RawPacket& raw) {
  raw >> timestamp;
}