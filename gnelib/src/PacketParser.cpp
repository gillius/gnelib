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
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Mutex.h"

//Packet type includes used for registration.
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/CustomPacket.h"
#include "../include/gnelib/ExitPacket.h"
#include "../include/gnelib/PingPacket.h"
#include "../include/gnelib/RateAdjustPacket.h"
#include "../include/gnelib/ObjectCreationPacket.h"
#include "../include/gnelib/ObjectUpdatePacket.h"
#include "../include/gnelib/ObjectDeathPacket.h"
#include "../include/gnelib/ChannelPacket.h"

namespace GNE {
namespace PacketParser {

static Mutex mapSync;

//Originally this was a map, but this array is only 2k, and its faster and
// easier to use a static array in this case.
static Packet* (*packets[256])();

/**
 * \todo GNE authors -- don't forget to add additional packets to this
 *       function as GNE expands.
 */
//This is a function defined only in GNE.cpp, as this is an internal init
// function.  The additions are hardcoded so registerPacket can do the proper
// asserts for the user's packets.  This function must also be called to
// properly init the parser.
void registerGNEPackets() {
  for (int c=0; c<256; c++) {
    packets[c] = NULL;
  }
  packets[0] = Packet::create;
  packets[1] = CustomPacket::create;
  packets[2] = ExitPacket::create;
  packets[3] = RateAdjustPacket::create;
  packets[4] = PingPacket::create;
  packets[5] = ChannelPacket::create;
  packets[6] = ObjectCreationPacket::create;
  packets[7] = ObjectUpdatePacket::create;
  packets[8] = ObjectDeathPacket::create;
}

void registerPacket(guint8 id, Packet* (*createFunc)()) {
  assert(id >= MIN_USER_ID && id <= MAX_USER_ID);
  assert( createFunc != Packet::create );

  mapSync.acquire();
  assert(packets[id] == NULL);
  packets[id] = createFunc;
  mapSync.release();
}

Packet* parseNextPacket(bool& endOfPackets, RawPacket& raw) {
  //Read next packet ID
  guint8 nextId;
  raw >> nextId;

  //Check for end of packet, returning if it is the end.
  if (nextId == END_OF_PACKET) {
    endOfPackets = true;
    return NULL;
  } else {
    endOfPackets = false;
  }

  //Check for packet registration, parsing if it is registered.
  Packet* (*func)() = NULL;

  mapSync.acquire();
  func = packets[nextId];
  mapSync.release();

  //This section is split off to keep the above critial section small as
  //many threads will be using this function, so we use this second if to
  //allow for better concurrency.
  if (func) {
    Packet* ret = func();
    ret->readPacket(raw);
    return ret;
  }
  gnedbg1(1, "Unknown packet type %i received.  onFailure event should occur.", (int)nextId);
  return NULL;
}

}
}





