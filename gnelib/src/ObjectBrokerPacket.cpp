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
#include "../include/gnelib/ObjectBrokerPacket.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {
  
ObjectBrokerPacket::ObjectBrokerPacket() : objectId( -1 ) {
}

ObjectBrokerPacket::ObjectBrokerPacket( int packetId, int objectId, const Packet* data )
: WrapperPacket( packetId, data ), objectId( objectId ) {
}

ObjectBrokerPacket::ObjectBrokerPacket( const ObjectBrokerPacket& o )
: WrapperPacket( o ), objectId( o.objectId ) {
}

ObjectBrokerPacket::~ObjectBrokerPacket() {
}

int ObjectBrokerPacket::getObjectId() const {
  return objectId;
}

void ObjectBrokerPacket::setObjectId( int newId ) {
  assert( newId >= 0 && newId <= 65535 );
  objectId = (guint16)newId;
}

int ObjectBrokerPacket::getSize() const {
  return WrapperPacket::getSize() + RawPacket::getSizeOf( objectId );
}

void ObjectBrokerPacket::writePacket(RawPacket& raw) const {
  WrapperPacket::writePacket( raw );
  raw << objectId;
}

void ObjectBrokerPacket::readPacket(RawPacket& raw) {
  WrapperPacket::readPacket( raw );
  raw >> objectId;
}

} //namespace GNE
