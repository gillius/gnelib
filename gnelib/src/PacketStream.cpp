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
#include "PacketStream.h"
#include "Packet.h"

namespace GNE {

//##ModelId=3B07538101BD
PacketStream::PacketStream(int outRate2, int inRate2)
: inRate(inRate2), outRate(outRate2) {
}

//##ModelId=3B07538101C0
PacketStream::~PacketStream() {
}

//##ModelId=3B07538101C2
int PacketStream::getInLength() const {
  return in.size();
}

//##ModelId=3B07538101C4
int PacketStream::getOutLength() const {
  return out.size();
}

//##ModelId=3B07538101C6
bool PacketStream::isNextPacket() const {
  return (in.size() != 0);
}

//##ModelId=3B07538101C8
Packet* PacketStream::getNextPacket() {
  return NULL;
}

//##ModelId=3B07538101C9
void PacketStream::writePacket(const Packet& packet, bool reliable) {
}

//##ModelId=3B07538101FD
Packet* PacketStream::getNextPacketToSend() {
  return NULL;
}

//##ModelId=3B07538101F5
int PacketStream::getInRate() const {
  return inRate;
}


//##ModelId=3B07538101F7
int PacketStream::getOutRate() const {
  return outRate;
}


//##ModelId=3B07538101F9
void PacketStream::waitToSendAll() {
}


//##ModelId=3B07538101FA
void PacketStream::run() {
}

//##ModelId=3B07538101FB
void PacketStream::addIncomingPacket(Packet* packet) {
}

}