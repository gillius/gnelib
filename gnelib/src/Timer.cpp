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
#include "Timer.h"
#include "Time.h"
#include "TimerCallback.h"

//##ModelId=3AE868370122
Timer::Timer(TimerCallback* callback, int rate, bool destroy)
: running(false), callbackRate(rate),
listener(callback), destroyListener(destroy) {
}

//##ModelId=3AE8686C00BE
Timer::~Timer() {
  stopTimer();
  if (destroyListener)
    delete listener;
}

/**
 * \todo write UNIX version
 */
//##ModelId=3AE86872030C
Time Timer::getCurrentTime() {
  Time ret;
#ifdef WIN32
  LARGE_INTEGER t, freq;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t);
  ret.setSec(int(t.QuadPart / freq.QuadPart));
  ret.setuSec(int((t.QuadPart % freq.QuadPart) * 1000000 / freq.QuadPart));
#else
#error Need to port Timer::getCurrentTime()
#endif
  return ret;
}

/**
 * \todo write UNIX version
 */
//##ModelId=3AF4797001CC
Time Timer::getAbsoluteTime() {
  Time ret;
#ifdef WIN32
  _timeb t;
  _ftime(&t);
  ret.setSec(t.time);
  ret.setuSec(t.millitm * 1000);
#else
#error Need to port Timer::getAbsoluteTime()
#endif
  return ret;
}

//##ModelId=3AEB9AB50050
void Timer::startTimer() {
  assert(running == false);
  nextTime = getCurrentTime();
  nextTime += callbackRate * 1000;
  running = true;
  start();
}

//##ModelId=3AEB9AB500BE
void Timer::stopTimer() {
  if (running) {
    running = false;
    join();
  }
}

//##ModelId=3AE868A30294
void Timer::run() {
  while (running) {
    Time currTime, sleepTime;
    currTime = getCurrentTime();
    while (nextTime > currTime) {
      sleepTime = nextTime - currTime;
      Thread::sleep(sleepTime.getTotaluSec() / 1000);
      currTime = getCurrentTime();
    }
    listener->timerCallback();
    nextTime += callbackRate * 1000;
  }
}

//##ModelId=3AED05E7029E
bool Timer::isRunning() {
  return running;
}
