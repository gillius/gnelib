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

const int PingPacket::ID = 4;
Mutex PingPacket::sync;
guint32 PingPacket::nextReqId = 0;
std::map<guint32, Time> PingPacket::requests;

PingPacket::PingPacket(bool makeReq) : Packet(ID), isReq(gTrue) {
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

PingPacket::~PingPacket() {
}

bool PingPacket::isRequest() {
  return (isReq != gFalse);
}

void PingPacket::makeReply() {
  isReq = gFalse;
}

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

int PingPacket::reqsPending() {
  sync.acquire();
  int ret = requests.size();
  sync.release();
  return ret;
}

Packet* PingPacket::makeClone() const {
  return new PingPacket(*this);
}

int PingPacket::getSize() const {
  return Packet::getSize() + sizeof(isReq) + sizeof(reqId);
}

void PingPacket::writePacket(RawPacket& raw) const {
  Packet::writePacket(raw);
  raw << isReq << reqId;
}

void PingPacket::readPacket(RawPacket& raw) {
  Packet::readPacket(raw);
  raw >> isReq >> reqId;
}

Packet* PingPacket::create() {
  return new PingPacket(false);
}

}
