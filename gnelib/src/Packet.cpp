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
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

const int Packet::ID = 0;

Packet::Packet(int id) : type((guint8)id), chan( 0 ) {
}

Packet::~Packet() {
}

Packet* Packet::makeClone() const {
  return new Packet(*this);
}

int Packet::getType() const {
  return (int)type;
}

int Packet::getChannel() const {
  return (int)chan;
}

void Packet::setChannel( int chan ) {
  assert( chan >= 0 && chan <= 255 );
  this->chan = chan;
}

int Packet::getSize() const {
  return RawPacket::getSizeOf(type) + RawPacket::getSizeOf( chan );
}

void Packet::writePacket(RawPacket& raw) const {
  raw << type;
  raw << chan;
}

void Packet::readPacket(RawPacket& raw) {
  raw >> chan;
}

Packet* Packet::create() {
  return new Packet();
}

Packet& Packet::operator = (const Packet& rhs) {
  //The type should already by the same
  assert(type == rhs.type);
  return *this;
}

}





