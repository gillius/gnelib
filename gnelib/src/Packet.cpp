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

#include "gneintern.h"
#include "Packet.h"
#include "RawPacket.h"

namespace GNE {

//##ModelId=3B0753810151
Packet::Packet(int id) : type((NLubyte)id) {
}

//##ModelId=3B0753810153
Packet::~Packet() {
}

//##ModelId=3B0753810155
Packet* Packet::makeClone() const {
  return new Packet(*this);
}

//##ModelId=3B0753810157
int Packet::getType() const {
  return (int)type;
}

//##ModelId=3B0753810159
int Packet::getSize() const {
  return sizeof(timestamp) + sizeof(type);
}

//##ModelId=3B075381015B
void Packet::writePacket(RawPacket& raw) const {
  raw << type << timestamp;
}

//##ModelId=3B075381015E
void Packet::readPacket(RawPacket& raw) {
  raw >> timestamp;
}

//##ModelId=3B089AE802BC
Packet* Packet::create() {
  return new Packet();
}

}


