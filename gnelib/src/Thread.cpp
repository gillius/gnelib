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

namespace GNE {

//##ModelId=3AE11D5F023A
std::map<pthread_t, Thread*> Thread::threads;
//##ModelId=3B0753810334
const int Thread::DEF_PRI = 0;
//##ModelId=3B0753810335
const int Thread::HIGH_PRI = 1;
//##ModelId=3AE1F1CA00DC
const std::string Thread::DEF_NAME = "Thread";
//##ModelId=3B0753810371
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

//##ModelId=3B0753810375
Thread::Thread() : name(DEF_NAME), thread_id(0), started(false), ended(false),
detached(false), priority(DEF_PRI) {
}

//##ModelId=3B0753810376
Thread::Thread(std::string name2, int priority2) : name(name2), thread_id(0),
started(false), ended(false), detached(false), priority(priority2) {
}

//##ModelId=3B0753810379
Thread::~Thread() {
}

//##ModelId=3B075381037B
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

//##ModelId=3B075381037D
void Thread::sleep(int ms) {
#ifdef WIN32
  Sleep(ms);
#else
  ConditionVariable sleeper;
  sleeper.acquire();
  sleeper.timedWait(ms);
  sleeper.release();
#endif
}

//##ModelId=3B0753810380
std::string Thread::getName() const {
  return name;
}

//##ModelId=3B0753810382
void Thread::join() {
  assert( started );
  valassert(pthread_join( thread_id, NULL ), 0);
}

//##ModelId=3B0753810383
void Thread::detach(bool deleteThis) {
  assert( started );
  assert( !detached );
  valassert(pthread_detach( thread_id ), 0);
  if (deleteThis) {
    //Only set detached true if we want to delete ourselves on exit.
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
}

//##ModelId=3B0753810387
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

//##ModelId=3B0753810388
bool Thread::isRunning() const {
  return started;
}

//##ModelId=3B07538103A3
bool Thread::hasEnded() const {
  return ended;
}

//##ModelId=3B07538103A5
void Thread::start() {
  started = true;
  pthread_create( &thread_id, NULL, threadStart, this );
  mapSync.acquire();
  threads[ thread_id ] = this;
  mapSync.release();
}

//##ModelId=3B07538103A6
int Thread::getPriority() const {
  return priority;
}

//##ModelId=3B07538103A8
pthread_t Thread::getID() const {
  return thread_id;
}

//##ModelId=3B07538103AA
void Thread::remove(Thread* thr) {
  mapSync.acquire();
  threads.erase(thr->getID());
  mapSync.release();
}

}