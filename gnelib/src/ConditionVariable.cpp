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
#include "../include/gnelib/ConditionVariable.h"
#include "../include/gnelib/Mutex.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Timer.h"

namespace GNE {

//##ModelId=3B07538003CD
ConditionVariable::ConditionVariable() {
  valassert(pthread_cond_init( &cond, NULL ), 0);
  ourMutex = true;
  mutex = new Mutex();
}

//##ModelId=3B07538003CE
ConditionVariable::ConditionVariable(Mutex* m) {
  valassert(pthread_cond_init( &cond, NULL ), 0);
  mutex = m;
  ourMutex = false;
}

//##ModelId=3B07538003D0
ConditionVariable::~ConditionVariable() {
  valassert(pthread_cond_destroy( &cond ), 0);
  if (ourMutex)
    delete mutex;
}

//##ModelId=3B0753810000
void ConditionVariable::acquire() {
  mutex->acquire();
}

//##ModelId=3B0753810001
void ConditionVariable::release() {
  mutex->release();
}

//##ModelId=3B0753810002
void ConditionVariable::wait() {
  valassert(pthread_cond_wait(&cond, &mutex->mutex), 0);
}

//##ModelId=3B0753810003
void ConditionVariable::timedWait(int ms) {
  Time t = Timer::getAbsoluteTime();
  t += ms*1000;
  timespec tv;
  tv.tv_sec = t.getSec();
  tv.tv_nsec = t.getuSec() * 1000;
  pthread_cond_timedwait(&cond, &(mutex->mutex), &tv);
}

//##ModelId=3B0753810005
void ConditionVariable::signal() {
  valassert(pthread_cond_signal( &cond ), 0);
}

//##ModelId=3B0753810006
void ConditionVariable::broadcast() {
  valassert(pthread_cond_broadcast( &cond ), 0);
}

}






