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
#include "../include/gnelib/PingPacket.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Mutex.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Timer.h"

namespace GNE {

//##ModelId=3C65C6D000C7
const int PingPacket::ID = 4;
//##ModelId=3C65C7A20259
Mutex PingPacket::sync;
//##ModelId=3C65C7A20263
guint32 PingPacket::nextReqId = 0;
std::map<guint32, Time> PingPacket::requests;

//##ModelId=3C65C6D000CD
PingPacket::PingPacket(bool makeReq) : isReq(gTrue) {
  //makeReq is false when creating a packet just for reading.
  if (makeReq) {
    sync.acquire();
    
    //Get a new, unique request ID.
    reqId = nextReqId++;
    
    //Then register it:
    assert(requests.find(reqId) == requests.end());
    requests[reqId] = Timer::getCurrentTime();
    
    sync.release();
  }
}

//##ModelId=3C65C6D000CF
PingPacket::~PingPacket() {
}

//##ModelId=3C65C6D000D1
bool PingPacket::isRequest() {
  return (isReq != gFalse);
}

//##ModelId=3C65C6D000D2
void PingPacket::makeReply() {
  isReq = gFalse;
}

//##ModelId=3C65C6D000D3
Time PingPacket::getPing() {
  assert(!isRequest());
  Time ret(0, 0);
  sync.acquire();

  std::map<guint32, Time>::iterator iter = requests.find(reqId);
  if (iter != requests.end()) {
    ret = Timer::getCurrentTime() - iter->second;
    requests.erase(iter);
  }

  sync.release();
  return ret;
}

//##ModelId=3C65C6D000D4
int PingPacket::recoverLostRequests(Time limit) {
  int ret = 0;
  Time late = Timer::getCurrentTime() - limit;
  sync.acquire();

  std::map<guint32, Time>::iterator iter = requests.begin();
  while (iter != requests.end()) {
    if (iter->second < late) {
      ++ret;
      requests.erase(iter++);
    } else
      ++iter;
  }

  sync.release();
  return ret;
}

//##ModelId=3C67292703D9
int PingPacket::reqsPending() {
  sync.acquire();
  int ret = requests.size();
  sync.release();
  return ret;
}

//##ModelId=3C65C6D000D7
Packet* PingPacket::makeClone() const {
  return new PingPacket(*this);
}

//##ModelId=3C65C6D000D9
int PingPacket::getSize() const {
  return Packet::getSize() + sizeof(isReq) + sizeof(reqId);
}

//##ModelId=3C65C6D000DB
void PingPacket::writePacket(RawPacket& raw) const {
  Packet::writePacket(raw);
  raw << isReq << reqId;
}

//##ModelId=3C65C6D000DE
void PingPacket::readPacket(RawPacket& raw) {
  Packet::readPacket(raw);
  raw >> isReq >> reqId;
}

//##ModelId=3C65C6D000E1
Packet* PingPacket::create() {
  return new PingPacket(false);
}

}
