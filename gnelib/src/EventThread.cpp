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
#include "../include/gnelib/EventThread.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/Thread.h"
#include "../include/gnelib/Timer.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/ConditionVariable.h"

namespace GNE {

EventThread::EventThread( const Connection::sptr& conn )
: Thread("EventThr", Thread::HIGH_PRI), ourConn(conn),
onReceiveEvent(false), onTimeoutEvent(false),
onDisconnectEvent(false), onExitEvent(false), failure(NULL) {
  gnedbgo(5, "created");
  setType( CONNECTION );
}

EventThread::sptr EventThread::create( const Connection::sptr& conn ) {
  sptr ret( new EventThread( conn ) );
  ret->setThisPointer( ret );
  return ret;
}

EventThread::~EventThread() {
  eventSync.acquire();
  while (!eventQueue.empty()) {
    delete eventQueue.front();
    eventQueue.pop();
  }
  eventSync.release();
  delete failure;
  gnedbgo(5, "destroyed");
}

ConnectionListener::sptr EventThread::getListener() const {
  listenSync.acquire();
  ConnectionListener::sptr ret = eventListener;
  listenSync.release();
  return ret;
}

void EventThread::setListener( const ConnectionListener::sptr& listener) {
  //Acquire listenSync to wait for the current event to complete.
  listenSync.acquire();

  //Acquire eventSync because of the wait loop check
  eventSync.acquire();
  eventListener = listener;

  //Signal the event thread in case it is waiting for a listener.
  eventSync.signal();
  eventSync.release();

  listenSync.release();
}

int EventThread::getTimeout() const {
  return (timeout.getuSec() / 1000);
}

void EventThread::setTimeout(int ms) {
  timeSync.acquire();
  if (ms != 0) {
    timeout = Time(0, ms * 1000);
    nextTimeout = Timer::getAbsoluteTime() + timeout;
  } else {
    nextTimeout = timeout = Time();
  }
  timeSync.release();

  //Wake up the event thread if it is sleeping, which is needed if there is
  //no timeout currently and the event thread is waiting forever on eventSync.
  eventSync.signal();
}

void EventThread::onDisconnect() {
  gnedbgo(1, "onDisconnect Event triggered.");
  //We acquire the mutex to avoid the possiblity of a deadlock between the
  // test for the shutdown variable and the wait.
  eventSync.acquire();
  onDisconnectEvent = true;
  eventSync.signal();
  eventSync.release();
}

void EventThread::onExit() {
  assert(failure == NULL); //only onFailure or onExit can happen.
  gnedbgo(1, "onExit Event triggered.");

  eventSync.acquire();
  onExitEvent = true;
  eventSync.signal();
  eventSync.release();
}

void EventThread::onFailure(const Error& error) {
  assert(!onExitEvent); //only onFailure or onExit can happen.
  gnedbgo1(1, "onFailure Event: %s", error.toString().c_str());

  eventSync.acquire();
  failure = new Error(error);
  eventSync.signal();
  eventSync.release();
}

void EventThread::onError(const Error& error) {
  gnedbgo1(1, "onError Event: %s", error.toString().c_str());

  eventSync.acquire();
  eventQueue.push(new Error(error));
  eventSync.signal();
  eventSync.release();
}

void EventThread::onReceive() {
  gnedbgo(4, "onReceive event triggered.");

  //reset the timeout counter
  resetTimeout();

  eventSync.acquire();
  onReceiveEvent = true;
  eventSync.signal();
  eventSync.release();
}

void EventThread::shutDown() {
  //Yep.  No setting of shutdown.  We want to try to close gracefully.  If we
  //can't do that we couldn't respond to shutdown either.
  ourConn->disconnect();

  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

void EventThread::run() {
  while ( true ) {
    //Yup.  No checking of shutdown.  When shutDown is called we call disconnect
    //on our connection, which should lead to a graceful shutdown.
    eventSync.acquire();
    //Wait while we have no listener and/or we have no events.
    while (!eventListener || (!onReceiveEvent && !failure &&
           !onDisconnectEvent && eventQueue.empty() &&
           !onExitEvent && !onTimeoutEvent) ) {
      //Calculate the time to wait
      if ( timeout == Time() ) {
        //wait "forever"
        eventSync.wait();
      } else {
        //wait until a timeout would occur
        eventSync.timedWait(nextTimeout);
        checkForTimeout();
      }
    }
    eventSync.release();

    checkForTimeout();

    //Check for events, processing them if events are pending
    if (failure) {
      listenSync.acquire();
      eventListener->onFailure(*failure);
      listenSync.release();
      ourConn->disconnect();
      delete failure;
      failure = NULL;

    } else if (onExitEvent) {
      onExitEvent = false;
      listenSync.acquire();
      eventListener->onExit();
      listenSync.release();
      ourConn->disconnect();

    } else if (onDisconnectEvent) {
      listenSync.acquire();
      eventListener->onDisconnect();
      listenSync.release();
      return;  //terminate this thread since there are no other events to
      //process -- onDisconnect HAS to be the last.

    } else if (onReceiveEvent) {
      //This is set to false before in case we get more packets during the
      //onReceive event.
      onReceiveEvent = false;
      listenSync.acquire();
      eventListener->onReceive();
      listenSync.release();

    } else if (onTimeoutEvent) {
      onTimeoutEvent = false;
      listenSync.acquire();
      eventListener->onTimeout();
      listenSync.release();

    } else {
      eventSync.acquire();

      //When we get here this is the only reason left why we were woken up!
      assert(!eventQueue.empty());
      Error* e = eventQueue.front();
      listenSync.acquire();
      eventListener->onError(*e);
      listenSync.release();
      delete e;
      eventQueue.pop();

      eventSync.release();
    }
  }
}

void EventThread::checkForTimeout() {
  LockMutexEx lock( timeSync );

  if ( timeout == Time() )
    return;

  Time t = nextTimeout;
  lock.release();

  if ( Timer::getAbsoluteTime() > t )
    onTimeout();
}

void EventThread::resetTimeout() {
  timeSync.acquire();
  if ( timeout != Time() ) {
    nextTimeout = Timer::getAbsoluteTime() + timeout;
  }
  timeSync.release();
}

void EventThread::onTimeout() {
  gnedbgo(4, "onTimeout event triggered.");

  //reset the timeout counter
  resetTimeout();

  eventSync.acquire();
  onTimeoutEvent = true;
  eventSync.signal();
  eventSync.release();
}

} //namespace GNE
