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
#include "Thread.h"
#include "ConditionVariable.h"

//##ModelId=3AE11D5F023A
std::map<pthread_t, Thread*> Thread::threads;
//##ModelId=3AE1EA0B02E4
const int Thread::DEF_PRI = 0;
//##ModelId=3AE359B501B8
const int Thread::HIGH_PRI = 1;
//##ModelId=3AE1F1CA00DC
const std::string Thread::DEF_NAME = "Thread";
//##ModelId=3AEE3C5D008D
Mutex Thread::mapSync;

extern "C" {
  /**
   * C helper function to start the thread.
   */
  static void* threadStart( void* thread ) {
    Thread* thr = ( ( Thread* )( thread ) );
    thr->run();
    thr->end();
    return NULL;
  }
}

//##ModelId=3AE12014000A
Thread::Thread() : name(DEF_NAME), thread_id(0), started(false), ended(false),
detached(false), priority(DEF_PRI) {
}

//##ModelId=3AE1224001CC
Thread::Thread(std::string name2, int priority2) : name(name2), thread_id(0),
started(false), ended(false), detached(false), priority(priority2) {
}

//##ModelId=3AE120140046
Thread::~Thread() {
}

//##ModelId=3AE126EF00D2
Thread* Thread::currentThread() {
  mapSync.acquire();
  std::map< pthread_t, Thread* >::iterator iter = threads.find( pthread_self() );
  if ( iter != threads.end() ) {
    Thread* ret = (*iter).second;
    mapSync.release();
    return ret;
  } else {
    mapSync.release();
    return NULL;
  }
}

//##ModelId=3AEEF76701E0
void Thread::sleep(int ms) {
  ConditionVariable sleeper;
  sleeper.acquire();
  sleeper.timedWait(ms);
  sleeper.release();
}

//##ModelId=3AE126FD00DC
std::string Thread::getName() const {
  return name;
}

//##ModelId=3AE1DFB40186
void Thread::join() {
  assert( started );
  valassert(pthread_join( thread_id, NULL ), 0);
}

//##ModelId=3AEE3C5D00BE
void Thread::detach() {
  assert( started );
  assert( !detached );
  valassert(pthread_detach( thread_id ), 0);
  sync.acquire();
  detached = true;
  if (ended) {
    detached = false; //this is just to fix a possible race condition with
                      //Thread::end.
    sync.release();
    delete this;
  } else
    sync.release();
}

//##ModelId=3AEE3C5D0168
bool Thread::isDetached() const {
  return detached;
}

//##ModelId=3AEE3C5D02E4
void Thread::end() {
  sync.acquire();
  Thread::remove(this);
  ended = true;
  if (detached) {
    ended = false; //this is just to fix a possible race condition with
                   //Thread::detach.
    sync.release();
    delete this;
  } else
    sync.release();
}

//##ModelId=3AEE3C5D0384
bool Thread::isRunning() const {
  return started;
}

//##ModelId=3AEE3C5E0154
bool Thread::hasEnded() const {
  return ended;
}

//##ModelId=3AE1DFC70230
void Thread::start() {
  started = true;
  pthread_create( &thread_id, NULL, threadStart, this );
  mapSync.acquire();
  threads[ thread_id ] = this;
  mapSync.release();
}

//##ModelId=3AE20B3402EE
int Thread::getPriority() const {
  return priority;
}

//##ModelId=3AE20B3F02E4
pthread_t Thread::getID() const {
  return thread_id;
}

//##ModelId=3AECE4A00046
void Thread::remove(Thread* thr) {
  mapSync.acquire();
  threads.erase(thr->getID());
  mapSync.release();
}



