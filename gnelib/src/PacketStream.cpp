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

//##ModelId=3AE3605F032A
PacketStream::PacketStream() : writer(writeSync) {
}

//##ModelId=3AE36060001E
PacketStream::~PacketStream() {
}

//##ModelId=3AE450930172
int PacketStream::getInLength() const {
  return in.size();
}

//##ModelId=3AE450AD019A
int PacketStream::getOutLength() const {
  return out.size();
}

//##ModelId=3AE44F5B0078
bool PacketStream::isNextPacket() const {
  return (in.size() != 0);
}

//##ModelId=3AE4503000B4
Packet* PacketStream::getNextPacket() {
  return NULL;
}

//##ModelId=3AE4503E023A
void PacketStream::writePacket(const Packet& packet, bool reliable) {
}

//##ModelId=3AE4CDD70122
Packet* PacketStream::getNextPacketToSend() {
  return NULL;
}

//##ModelId=3AE4C60C026C
int PacketStream::getInRate() const
{
    return inRate;
}


//##ModelId=3AE4C60C0316
int PacketStream::getOutRate() const
{
    return outRate;
}


//##ModelId=3AE4C70203C0
void PacketStream::waitToSendAll() {
}


