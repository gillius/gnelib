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
#include "../include/gnelib/RateAdjustPacket.h"
#include "../include/gnelib/ExitPacket.h"
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Timer.h"
#include "../include/gnelib/Errors.h"

const int BUF_LEN = 1024;

//This is the timestep size for the rate control in microseconds.
//The current step is 1/10 of a second which allows for a rate granularity of
//10 bps.
const int TIME_STEP = 100000;
const int TIME_STEPS_PER_SEC = 1000000 / TIME_STEP;

namespace GNE {

//##ModelId=3B07538101BD
PacketStream::PacketStream(int reqOutRate2, int maxOutRate2, Connection& ourOwner)
: Thread("PktStrm"), owner(ourOwner), maxOutRate(maxOutRate2),
reqOutRate(reqOutRate2) {
  assert(reqOutRate2 >= 0);
  assert(maxOutRate2 >= 0);

  //Calculate the current rate and step.
  setupCurrRate();

  //Set the amount we can send without waiting to the max.
  outRemain = currOutRate;

  //Set the last calculation time:
  lastTime = Timer::getCurrentTime();

  gnedbgo2(2, "PacketStream negotiated: max: %d requested: %d",
    maxOutRate, reqOutRate);
  gnedbgo(5, "created");
}

//##ModelId=3B07538101C0
PacketStream::~PacketStream() {
  //We can't do assert(shutdown) in case PacketStream was never started.
  assert(!isRunning());

  //Clear out our queues.
  Packet* temp = NULL;

  //Empty out the outgoing queues.
  outQCtrl.acquire();
  while (!outRel.empty()) {
    delete outRel.front();
    outRel.pop();
  }
  while (!outUnrel.empty()) {
    delete outUnrel.front();
    outUnrel.pop();
  }
  outQCtrl.release();

  //Empty the incoming queue.
  while ((temp = getNextPacket()) != NULL)
    delete temp;

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
int PacketStream::getOutLength(bool reliable) const {
  int ret;
  outQCtrl.acquire();

  if (reliable)
    ret = outRel.size();
  else
    ret = outUnrel.size();

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
  //Perform operations on the outgoing queue
  outQCtrl.acquire();
  bool notify = false;
  if (reliable) {
    notify = outRel.empty();
    outRel.push(packet.makeClone());
  } else {
    notify = outUnrel.empty();
    outUnrel.push(packet.makeClone());
  }
  outQCtrl.release();

  //If we need to, wake up the writer thread.
  if (notify)
    outQCtrl.broadcast();
}

//##ModelId=3B07538101F7
int PacketStream::getCurrOutRate() const {
  return currOutRate;
}

//##ModelId=3C7AB4C501C1
int PacketStream::getReqOutRate() const {
  return reqOutRate;
}

//##ModelId=3C7AB4C501CB
int PacketStream::getRemoteOutLimit() const {
  return maxOutRate;
}

//##ModelId=3C783ACF0264
void PacketStream::setRates(int reqOutRate2, int maxInRate2) {
  if (reqOutRate2 >= 0) {
    outQCtrl.acquire();
    reqOutRate = reqOutRate2;
    setupCurrRate();
    outQCtrl.release();
  }

  //Now handle the inRate changes, sending a notice if needed.
  if (maxInRate2 >= 0) {
    RateAdjustPacket notice;
    notice.rate = maxInRate2;
    writePacket(notice, true);
  }
}

//##ModelId=3B07538101F9
void PacketStream::waitToSendAll(int waitTime) const {
  assert(waitTime <= (INT_MAX / 1000));
  assert(waitTime > 0);

  Time t = Timer::getCurrentTime();
  Time waitUntil = t + (waitTime * 1000);
  bool timeOut = (waitTime <= 0);
  int ms = waitTime;

  outQCtrl.acquire();
  while ((!outRel.empty() || !outUnrel.empty()) && !shutdown && !timeOut) {
    outQCtrl.timedWait(ms);

    t = Timer::getCurrentTime();
    timeOut = (t > waitUntil);
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
 * \bug  we need a way to guarantee that the write won't block when trying
 *       to send the ExitPacket on shutdown.
 */
//##ModelId=3B07538101FA
void PacketStream::run() {
  outQCtrl.acquire();
  while (!shutdown) {
    while (outRel.empty() && outUnrel.empty() && !shutdown) {
      outQCtrl.wait();
    }
    //Check which queue woke us up.  If they are both empty it will not
    //matter as this means shutdown is true.  Doing the check this way gives
    //absolute priority to reliable packets.
    bool reliable = !outRel.empty();

    if (!shutdown) {
      //Do throttled writes
      updateRates();
      if (outRemain > 0) {
        //Yes, this check will let us dip below 0, but overall we will make
        //up for it by waiting for it to go above 0 again.
        RawPacket raw;
        prepareSend( ((reliable) ? outRel : outUnrel), raw);
        raw << PacketParser::END_OF_PACKET;
        outRemain -= raw.getPosition();

        //Release the mutex in case rawWrite blocks
        outQCtrl.release();
        if (owner.sockets.rawWrite(reliable, raw.getData(), raw.getPosition()) == NL_INVALID) {
          owner.processError( LowLevelError(Error::Write) );
        }
        
        //Discover if we are really done writing, and if so, queue the
        //onDoneWriting event.
        outQCtrl.acquire();
        if (outRel.empty() && outUnrel.empty()) {
          //Broadcast a wakeup in case someone is waiting in waitToSendAll.
          outQCtrl.broadcast();
          owner.onDoneWriting();
        }
      } else {
        //Else we don't have any available bandwidth and we must wait!
        outQCtrl.release();
        Thread::sleep(TIME_STEP / 1000);
        outQCtrl.acquire();
      }
    }
  }
  outQCtrl.release();

  //We want to try to send the required ExitPacket, if possible, over the
  //reliable connection.
  //We need a good way to make sure this doesn't block though, but the
  //current solution here of assuming rawWrite won't block will have to do
  //for now.
  RawPacket raw;
  ExitPacket temp;
  raw << temp << PacketParser::END_OF_PACKET;
  int ret = owner.sockets.rawWrite(true, raw.getData(), raw.getPosition());
  if (ret > 0) {
    gnedbgo1(4, "ExitPacket sent (%d).", ret);
  } else {
    gnedbgo1(4, "ExitPacket not sent (%d).", ret);
  }
}

//##ModelId=3B07538101FB
void PacketStream::addIncomingPacket(Packet* packet) {
  if (packet->getType() != RateAdjustPacket::ID) {
    inQCtrl.acquire();
    in.push(packet);
    inQCtrl.release();
  } else {
    //We want to "intercept" RateAdjustPackets
    outQCtrl.acquire();
    maxOutRate = ((RateAdjustPacket*)packet)->rate;
    gnedbgo1(2, "Received new outgoing rate limit of %d", maxOutRate);
    setupCurrRate();
    outQCtrl.release();
  }
}

//##ModelId=3C7867230185
void PacketStream::prepareSend(std::queue<Packet*>& q, RawPacket& raw) {
  //outQCtrl must be acquired for this function.
  //While there are packets left and they won't overflow the RawPacket
  while (!q.empty() &&
         raw.getPosition() + q.front()->getSize() <
         RawPacket::RAW_PACKET_LEN - (int)sizeof(PacketParser::END_OF_PACKET)) {

    q.front()->writePacket(raw);
    delete q.front();
    q.pop();
  }
}

//##ModelId=3C7960970177
void PacketStream::setupCurrRate() {
  //Precalculate the current outgoing rate, keeping in mind that the value of
  //is the "largest" and means unlimited rate (or "unchecked").  Unlimited is
  //OK only if they are both at 0.  outQCtrl should be acquired when this
  //function is running.
  if (reqOutRate == 0)
    currOutRate = maxOutRate;
  else if (maxOutRate == 0)
    currOutRate = reqOutRate;
  else
    currOutRate = (reqOutRate < maxOutRate) ? reqOutRate : maxOutRate;

  //Discover the rate stepping.
  outRateStep = currOutRate / TIME_STEPS_PER_SEC;

  gnedbgo2(2, "  Negotiated current rate: %d, rate step: %d",
    currOutRate, outRateStep);
}

//##ModelId=3C783ACF028C
void PacketStream::updateRates() {
  //The out remain is part of the out queue, so outQCtrl must be locked when
  //we call this function.

  if (currOutRate > 0) {
    //Find the time difference in number of steps
    int timeDiff =
      (Timer::getCurrentTime() - lastTime).getTotaluSec() / TIME_STEP;
    
    outRemain += outRateStep * timeDiff;
    //Because of the math here, we compensate/take advatnage of integer
    //rounding.
    lastTime += TIME_STEP * timeDiff;
    //We won't allow large bursts of data -- no more than 1 second's worth.
    if (outRemain > currOutRate)
      outRemain = currOutRate;

  } else {
    //Else, we are not rate limiting, so we set outRemain to some fake value.
    outRemain = 1;
  }
}

}
