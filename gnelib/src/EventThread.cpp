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
#include "../include/gnelib/Thread.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/ConditionVariable.h"

namespace GNE {

//##ModelId=3C106F0203D4
EventThread::EventThread(ConnectionListener* listener)
: Thread("EventThr"), started(false), eventListener(listener),
onReceiveEvent(false), onDoneWritingEvent(false), onDisconnectEvent(false),
failure(NULL) {
  gnedbgo(5, "created");
}

//##ModelId=3C106F0203D6
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

//##ModelId=3C106F0203D8
ConnectionListener* EventThread::getListener() const {
  return eventListener;
}

//##ModelId=3C106F0203DA
void EventThread::setListener(ConnectionListener* listener) {
  eventListener = listener;
}

//##ModelId=3C106F0203DC
void EventThread::onDisconnect() {
  assert(eventListener != NULL);
  gnedbgo(1, "onDisconnect Event triggered.");
  onDisconnectEvent = true;
  //We acquire the mutex to avoid the possiblity of a deadlock between the
  // test for the shutdown variable and the wait.
  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203DD
void EventThread::onFailure(const Error& error) {
  assert(eventListener != NULL);
  gnedbgo1(1, "onFailure Event: %s", error.toString().c_str());

  eventSync.acquire();
  failure = new Error(error);
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203DF
void EventThread::onError(const Error& error) {
  assert(eventListener != NULL);
  gnedbgo1(1, "onError Event: %s", error.toString().c_str());

  eventSync.acquire();
  eventQueue.push(new Error(error));
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E1
void EventThread::onReceive() {
  assert(eventListener != NULL);
  gnedbgo(4, "onReceive event triggered.");
  onReceiveEvent = true;

  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E2
void EventThread::onDoneWriting() {
  assert(eventListener != NULL);
  gnedbgo(4, "onDoneWriting event triggered.");
  onDoneWritingEvent = true;

  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E3
void EventThread::shutDown() {
  Thread::shutDown();

  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E4
bool EventThread::hasStarted() {
  return started;
}

//##ModelId=3C106F0203E5
void EventThread::start() {
  Thread::start();
  started = true;
}

//##ModelId=3C106F0203E6
void EventThread::run() {
  while (!shutdown) {
    eventSync.acquire();
    while (!onReceiveEvent && !onDoneWritingEvent && !failure
           && !onDisconnectEvent && eventQueue.empty() && !shutdown) {
      eventSync.wait();
    }
    eventSync.release();
    if (!shutdown) {
      if (failure) {
        eventListener->onFailure(*failure);
        delete failure;
        failure = NULL;

      } else if (onDisconnectEvent) {
        eventListener->onDisconnect();
        onDisconnectEvent = false;
        shutDown(); //Make sure NO other events are called.

      } else if (onReceiveEvent) {
        //This is set to false before in case we get more packets during the
        //onReceive event.
        onReceiveEvent = false;
        eventListener->onReceive();

      } else if (onDoneWritingEvent) {
        //This is set before in case the packets finish writing during the
        //onDoneWriting event.
        onDoneWritingEvent = false;
        eventListener->onDoneWriting();

      } else {
        eventSync.acquire();

        //When we get here this is the only reason left why we were woken up!
        assert(!eventQueue.empty());
        Error* e = eventQueue.front();
        eventListener->onError(*e);
        delete e;
        eventQueue.pop();

        eventSync.release();
      }
    }
  }
}

} //namespace GNE
