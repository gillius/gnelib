#ifndef _EVENTTHREAD_H_
#define _EVENTTHREAD_H_

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

#include "gneintern.h"
#include "Thread.h"
#include "Error.h"
#include "ConditionVariable.h"

namespace GNE {
class ConnectionListener;
class Connection;

/**
 * Each Connection has an EventThread.  This is used internally by the
 * Connection class to act as a proxy to dispatch events to the
 * ConnectionListener.  Because of this, only one event per Connection will
 * be active at any one time.  Another appropriate name that might describe
 * this would be an EventQueue.  But to improve efficency and stability,
 * some events may be reordered or combined.  Remember that onReceive means 1
 * or more packets have been received.  If multiple onReceive or
 * onDoneWriting occur, they will probably be combined.  Error events will
 * always have first priority, and if the error leads to a disconnect,
 * pending events after taht will not be called (except for onDisconnect).\n
 * EventThread was created to solve several things:\n
 * = Complexities due to multiple threads calling the ConnectionListener at
 *   the same time.\n
 * = Serialized events and all events executing in the same thread is easy to
 *   control and eliminates lots of previous ways for syncronization errors
 *   to creep in.\n
 * = Events taking a while to execute or block (although this is an error),
 *   won't stop the rest of GNE from functioning, but will only stop a single
 *   Connection.\n
 * = Multiple event threads take better advantage of multiprocessor machines.
 */
//##ModelId=3C106F0202CD
class EventThread : public Thread {
public:
  /**
   * Initializes this class as a event thread for listener.
   */
  //##ModelId=3C106F0203D4
  EventThread(ConnectionListener* listener = NULL);

  //##ModelId=3C106F0203D6
  virtual ~EventThread();

  /**
   * Returns the currently registered event Listener.  This can be useful to
   * delete your registered user right after disconnection, so you won't have
   * to keep track of the pointer after you allocate it.
   */
  //##ModelId=3C106F0203D8
  ConnectionListener* getListener() const;
  
  /**
   * Sets a new event listener.  All new events will be sent to the new
   * listener.  If events are still being processed by the old listener, this
   * function will wait until that event completes, then it will release.
   * At the completion of this statement you know for certain that the old
   * listener is not being addressed by this thread any longer.\n
   * If this function is called from an event itself, then the change will
   * take place at the start of the next event.
   * The passed listener cannot be NULL.
   */
  //##ModelId=3C106F0203DA
  void setListener(ConnectionListener* listener);

  /**
   * For more information about these events, see ConnectionListener.  The
   * processing of an onDisconnect event will be the last, and the thread
   * will essentially stop when onDisconnect completes.
   */
  //##ModelId=3C106F0203DC
  void onDisconnect();

  /**
   * For more information about these events, see ConnectionListener.
   */
  //##ModelId=3C106F0203DD
  void onFailure(const Error& error);

  /**
   * For more information about these events, see ConnectionListener.
   */
  //##ModelId=3C106F0203DF
  void onError(const Error& error);

  /**
   * For more information about these events, see ConnectionListener.
   */
  //##ModelId=3C106F0203E1
  void onReceive();

  /**
   * For more information about these events, see ConnectionListener.
   */
  //##ModelId=3C106F0203E2
  void onDoneWriting();

  /**
   * Overrides Thread::shutDown so that the daemon thread will
   * be woken up since it might be waiting on a ConditionVariable.  Once it
   * shuts down it should not be activated again.
   */
  //##ModelId=3C106F0203E3
  void shutDown();

  /**
   * Returns true if this EventThread was ever started at least once.
   */
  //##ModelId=3C106F0203E4
  bool hasStarted();

  /**
   * Starts this EventThread.
   */
  //##ModelId=3C106F0203E5
  void start();

protected:
  /**
   * This thread serializes events for a Connection.
   */
  //##ModelId=3C106F0203E6
  void run();

private:
  //The listener for our events.  All on* events go here.  This is protected
  //so ClientConnection can send events as well.
  //##ModelId=3C106F1903BE
  ConnectionListener* eventListener;

  //##ModelId=3C106F1903C3
  ConditionVariable eventSync;

  //##ModelId=3C4116C301A9
  ConditionVariable listenSync;

  //True if this EventThread was ever started at least one time.
  //##ModelId=3C106F0203CB
  bool started;

  //##ModelId=3C106F0203CC
  bool onReceiveEvent;
  //##ModelId=3C106F0203CD
  bool onDoneWritingEvent;

  //If this is true, we should not receive any more events.  It should be the
  //next event called, and everything else should stop.
  //##ModelId=3C106F0203CE
  bool onDisconnectEvent;

  //Wish we could combine error events and report the last error, but this
  //really isn't what the API specifies, so we should keep track of every
  //error.
  std::queue<Error*> eventQueue;

  //If this is true, we have a onFailure event which takes precedence over
  //everything.
  //##ModelId=3C106F1903CA
  Error* failure;
};

}
#endif /* _EVENTTHREAD_H_ */
