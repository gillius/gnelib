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
#include "../include/gnelib/PacketStream.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Timer.h"

const int BUF_LEN = 1024;

namespace GNE {

//##ModelId=3B07538101BD
PacketStream::PacketStream(int outRate2, int inRate2, Connection& ourOwner)
: Thread("PktStrm"), owner(ourOwner), inRate(inRate2), outRate(outRate2) {
  gnedbgo(5, "created");
}

//##ModelId=3B07538101C0
PacketStream::~PacketStream() {
  //We can't do assert(shutdown) in case PacketStream was never started.
  assert(!isRunning());

  //Clear out our queues.
  PacketStreamData* outQ = NULL;
  Packet* inQ = NULL;
  while ((outQ = getNextPacketToSend()) != NULL)
    delete outQ;
  while ((inQ = getNextPacket()) != NULL)
    delete inQ;

  gnedbgo(5, "destroyed");
}

//##ModelId=3B07538101C2
int PacketStream::getInLength() const {
  int ret;
  inQCtrl.acquire();
  ret = in.size();
  inQCtrl.release();
  return ret;
}

//##ModelId=3B07538101C4
int PacketStream::getOutLength() const {
  int ret;
  outQCtrl.acquire();
  ret = out.size();
  outQCtrl.release();
  return ret;
}

//##ModelId=3B07538101C6
bool PacketStream::isNextPacket() const {
  return (getInLength() != 0);
}

//##ModelId=3B07538101C8
Packet* PacketStream::getNextPacket() {
  Packet* ret = NULL;
  inQCtrl.acquire();
  if (!in.empty()) {
    ret = in.front();
    in.pop();
  }
  inQCtrl.release();
  return ret;
}

//##ModelId=3B07538101C9
void PacketStream::writePacket(const Packet& packet, bool reliable) {
  //Create the new data to be in the queue
  PacketStreamData* newNode = new PacketStreamData;
  newNode->reliable = reliable;
  newNode->packet = packet.makeClone();

  //Perform operations on the outgoing queue
  outQCtrl.acquire();
  bool notify = out.empty();
  out.push(newNode);
  outQCtrl.release();

  //If we need to, wake up the writer thread.
  if (notify)
    outQCtrl.signal();
}

//##ModelId=3B07538101FD
PacketStream::PacketStreamData* PacketStream::getNextPacketToSend() {
  PacketStream::PacketStreamData* ret = NULL;
  outQCtrl.acquire();
  if (!out.empty()) {
    ret = out.front();
    out.pop();
  }
  outQCtrl.release();
  return ret;
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
void PacketStream::waitToSendAll(int waitTime) {
  assert(waitTime < (INT_MAX / 1000));
  assert(waitTime > 0);

  Time t = Timer::getCurrentTime();
  Time waitUntil = t + (waitTime * 1000);
  bool timeOut = (waitTime <= 0);
  int ms = waitTime;

  outQCtrl.acquire();
  while (!out.empty() && !shutdown && !timeOut) {
    outQCtrl.timedWait(ms);

    t = Timer::getCurrentTime();
    timeOut = (t < waitUntil);

    if (timeOut)
      ms = (t - waitUntil).getTotaluSec() / 1000;
  }
  outQCtrl.release();
}

//##ModelId=3B8DC5D10096
void PacketStream::shutDown() {
  Thread::shutDown();
  //We acquire the mutex to avoid the possiblity of a deadlock between the
  // test for the shutdown variable and the wait.
  outQCtrl.acquire();
  outQCtrl.broadcast();
  outQCtrl.release();
}

/**
 * \todo combine packets, do throttling, optimize
 * \bug  we need a way to guarantee that the write won't block when trying
 *       to send the ExitPacket on shutdown.
 */
//##ModelId=3B07538101FA
void PacketStream::run() {
  while (!shutdown) {
    outQCtrl.acquire();
    while (out.empty() && !shutdown) {
      outQCtrl.wait();
    }
    outQCtrl.release();
    if (!shutdown) {
      //Do throttled writes
      PacketStreamData* next = getNextPacketToSend();
      RawPacket raw;
      next->packet->writePacket(raw);
      raw << PacketParser::END_OF_PACKET;
      if (owner.sockets.rawWrite(next->reliable, raw.getData(), raw.getPosition()) == NL_INVALID) {
        owner.processError(Error::Write);
      }
      delete next->packet;
      delete next;
      
      //Discover if we are really done writing, and if so, queue the
      //onDoneWriting event.
      outQCtrl.acquire();
      if (out.empty()) {
        //Broadcast a wakeup in case someone is waiting in waitToSendAll.
        outQCtrl.broadcast();
        owner.onDoneWriting();
      }
      outQCtrl.release();
    }
  }
  //We want to try to send the required ExitPacket, if possible, over the
  //reliable connection.
  //We need a good way to make sure this doesn't block though, but the
  //current solution here will have to do for now.
  RawPacket raw;

}

//##ModelId=3B07538101FB
void PacketStream::addIncomingPacket(Packet* packet) {
  inQCtrl.acquire();
  in.push(packet);
  inQCtrl.release();
}

}
