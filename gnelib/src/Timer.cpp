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

//##ModelId=3AEBA397028B
Time Timer::startTime;

//##ModelId=3AE868370122
Timer::Timer(TimerCallback& theListener, int rate)
: running(false), callbackRate(rate), listener(theListener) {
}

//##ModelId=3AE8686C00BE
Timer::~Timer() {
}

#ifdef WIN32
#include <sys/timeb.h>
#endif

//##ModelId=3AE86872030C
Time Timer::getCurrentTime() {
  Time ret;
#ifdef WIN32
  _timeb t;
  _ftime(&t);
  ret.setSec(t.time);
  ret.setuSec(t.millitm * 1000);
#else
#error Need to port Timer::getCurrentTime()
#endif
  return ret;
}

//##ModelId=3AEB9AB4038E
void Timer::initTime() {
}

//##ModelId=3AEB9AB50050
void Timer::startTimer() {
}

//##ModelId=3AEB9AB500BE
void Timer::stopTimer() {
}

//##ModelId=3AE868A30294
void Timer::run() {
}

//##ModelId=3AED05E7029E
bool Timer::isRunning() {
  return running;
}

