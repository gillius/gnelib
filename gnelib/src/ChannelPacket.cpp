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
#include "../include/gnelib/ChannelPacket.h"
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

const int ChannelPacket::ID = 5;

ChannelPacket::ChannelPacket( int channel, int from, const Packet& packet )
: Packet(ID), channel((guint8)channel), from((guint8)from) {
  msg = packet.makeClone();
}

ChannelPacket::~ChannelPacket() {
  delete msg;
}

int ChannelPacket::getChannel() const {
  return channel;
}

void ChannelPacket::setChannel( int chan ) {
  assert( channel >= 0 && channel <= 255 );
  channel = (guint8)chan;
}

int ChannelPacket::getSource() const {
  return from;
}

void ChannelPacket::setSource( int source ) {
  assert( source >= 0 && source <= 255 );
  from = (guint8)source;
}

const Packet& ChannelPacket::getData() const {
  return *msg;
}

void ChannelPacket::setData( const Packet& packet ) {
  delete msg;
  msg = packet.makeClone();
}

Packet* ChannelPacket::makeClone() const {
  return new ChannelPacket( channel, from, *msg );
}

int ChannelPacket::getSize() const {
  return RawPacket::getSizeOf( channel ) +
    RawPacket::getSizeOf( from ) + msg->getSize();
}

void ChannelPacket::writePacket(RawPacket& raw) const {
  raw << channel << from;
  msg->writePacket( raw );
}

void ChannelPacket::readPacket(RawPacket& raw) {
  raw >> channel >> from;
  bool dummy;
  msg = PacketParser::parseNextPacket( dummy, raw );
  assert( dummy == false );
  assert( msg != NULL );
}

Packet* ChannelPacket::create() {
  return new ChannelPacket;
}

} //namespace GNE
