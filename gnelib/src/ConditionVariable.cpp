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
#include "ConditionVariable.h"
#include "Mutex.h"
#include "Time.h"
#include "Timer.h"

//##ModelId=3AE20E490140
ConditionVariable::ConditionVariable() {
  valassert(pthread_cond_init( &cond, NULL ), 0);
  ourMutex = true;
  mutex = new Mutex();
}

//##ModelId=3AE50F1A0398
ConditionVariable::ConditionVariable(Mutex* m) {
  valassert(pthread_cond_init( &cond, NULL ), 0);
  mutex = m;
  ourMutex = false;
}

//##ModelId=3AE20E880104
ConditionVariable::~ConditionVariable() {
  valassert(pthread_cond_destroy( &cond ), 0);
  if (ourMutex)
    delete mutex;
}

//##ModelId=3AE50F1B00FA
void ConditionVariable::acquire() {
  mutex->acquire();
}

//##ModelId=3AE50F1B0168
void ConditionVariable::release() {
  mutex->release();
}

//##ModelId=3AE20E5F02DA
void ConditionVariable::wait() {
  valassert(pthread_cond_wait(&cond, &mutex->mutex), 0);
}

//##ModelId=3AEEF7660032
void ConditionVariable::timedWait(int ms) {
  Time t = Timer::getAbsoluteTime();
  t += ms*1000;
  timespec tv;
  tv.tv_sec = t.getSec();
  tv.tv_nsec = t.getuSec() * 1000;
  mutex->acquire();
  pthread_cond_timedwait(&cond, &(mutex->mutex), &tv);
  mutex->release();
}

//##ModelId=3AE20E7200B4
void ConditionVariable::signal() {
  valassert(pthread_cond_signal( &cond ), 0);
}

//##ModelId=3AE20E7A00FA
void ConditionVariable::broadcast() {
  valassert(pthread_cond_broadcast( &cond ), 0);
}


