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
#include "../include/gnelib/Thread.h"
#include "../include/gnelib/ConditionVariable.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Timer.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/GNE.h"

namespace GNE {

//##ModelId=3AE11D5F023A
std::map<Thread::ID, Thread*> Thread::threads;
//##ModelId=3BB805C6014B
Mutex Thread::mapSync;
//##ModelId=3B0753810334
const int Thread::DEF_PRI = 0;
//##ModelId=3CBD09C8004F
const int Thread::LOW_PRI = -1;
//##ModelId=3CBD09C8006D
const int Thread::LOWER_PRI = -2;
//##ModelId=3B0753810335
const int Thread::HIGH_PRI = 1;
//##ModelId=3CBD09C80081
const int Thread::HIGHER_PRI = 2;
//##ModelId=3AE1F1CA00DC
const std::string Thread::DEF_NAME = "Thread";

//##ModelId=3BB805C60186
Thread::RETCODE THREAD_CALLTYPE Thread::threadStart( void* thread ) {
  Thread* thr = ( ( Thread* )( thread ) );
  //Makes sure the map has been updated before we start.
  mapSync.acquire();
  mapSync.release();

  try {
    thr->run();
    thr->end();
  } catch (Error& e) {
    gnedbg2(1, "Unhandled exception, thread terminated. Error %d: %s",
      e.getCode(), e.toString().c_str());
  }
  //We don't do a catch all because interestingly enough, the MSVC debugger
  //is started by throwing an exception, and placing a catch all here will
  //keep the debugger from starting.

  return 0;
}

//##ModelId=3B0753810376
Thread::Thread(std::string name2, int priority2) : shutdown(false), name(name2),
thread_id(0), running(false), deleteThis(false), priority(priority2) {
#ifdef WIN32
  hThread = 0;
#endif
}

//##ModelId=3B0753810379
Thread::~Thread() {
  assert(!isRunning());
#ifdef WIN32
  CloseHandle( hThread );
#endif
}

//##ModelId=3B075381037B
Thread* Thread::currentThread() {
  mapSync.acquire();
#ifdef WIN32
  ID id = GetCurrentThreadId();
#else
  ID id = pthread_self();
#endif

  std::map< ID, Thread* >::iterator iter = threads.find( id );
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
  assert(ms > 0);
  if (ms > 0) {
#ifdef WIN32
    Sleep(ms);
#else
    ConditionVariable sleeper;
    sleeper.acquire();
    sleeper.timedWait(ms);
    sleeper.release();
#endif
  }
}

//##ModelId=3D1F80610065
void Thread::yield() {
#ifdef WIN32
  Sleep(0);
#else
  sched_yield();
#endif
}

//##ModelId=3C885B3800E8
bool Thread::waitForAllThreads(int ms) {
  if (ms > INT_MAX / 1000)
    ms = INT_MAX / 1000;

  Time t = Timer::getCurrentTime();
  t += ms * 1000;

  bool ret = false;
  bool timeout = false;
  while (!ret) {
    ret = timeout = (Timer::getCurrentTime() < t);
    if (!timeout) {
      mapSync.acquire();
      //Take into accout the CEG thread.
      ret = (threads.size() <= (unsigned int)((GNE::eGen) ? 1 : 0) );
      mapSync.release();
    }
    if (!ret)
      sleep(20);
  }

  return timeout;
}

//##ModelId=3B0753810380
std::string Thread::getName() const {
  return name;
}

//##ModelId=3B0C5919012C
void Thread::shutDown() {
  shutdown = true;
}

//##ModelId=3B0753810382
void Thread::join() {
  assert( !deleteThis );
#ifdef WIN32
  valassert(WaitForSingleObject( hThread, INFINITE ), WAIT_OBJECT_0);
#else
  valassert(pthread_join( thread_id, NULL ), 0);
#endif
}

//##ModelId=3B0753810383
void Thread::detach(bool delThis) {
  assert( !deleteThis );
#ifndef WIN32
  //We only need to detach on POSIX systems
  valassert(pthread_detach( thread_id ), 0);
#endif
  if (delThis) {
    //Only set deleteThis true if we want to delete ourselves on exit.
    sync.acquire();
    deleteThis = true;
    if (!running) {       //delete this if we are already stopped.
      sync.release();
      delete this;
    } else                //else all we want to do is mark deleteThis.
      sync.release();
  }
}

//##ModelId=3B0753810387
void Thread::end() {
  sync.acquire();
  running = false;
  Thread::remove(this);
  if (deleteThis) {
    //If we were detached with delThis set, we delete ourselves now.
    sync.release();
    delete this;
  } else
    sync.release();
}

//##ModelId=3B0753810388
bool Thread::isRunning() const {
  return running;
}

//##ModelId=3B07538103A5
void Thread::start() {
  shutdown = false;
  running = true;
  mapSync.acquire();

#ifdef WIN32
  hThread = (HANDLE)_beginthreadex( NULL, 0, &threadStart, (void*)this, 0,
                                    reinterpret_cast<unsigned*>(&thread_id) );
  //Set the thread priority
  switch (priority) {
    case LOWER_PRI:
      SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
      break;
    case LOW_PRI:
      SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
      break;
    case HIGH_PRI:
      SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
      break;
    case HIGHER_PRI:
      SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
      break;
  };
#else
  pthread_create( &thread_id, NULL, Thread::threadStart, this );
#endif

  threads[ thread_id ] = this;
  mapSync.release();
}

//##ModelId=3B07538103A6
int Thread::getPriority() const {
  return priority;
}

//##ModelId=3B07538103AA
void Thread::remove(Thread* thr) {
  assert(!thr->isRunning());
  mapSync.acquire();
  threads.erase(thr->thread_id);
  mapSync.release();
}

}





