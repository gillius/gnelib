#ifndef TIMER_H_INCLUDED_C517B9FE
#define TIMER_H_INCLUDED_C517B9FE

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
#include "Time.h"
#include "Mutex.h"

namespace GNE {
class TimerCallback;

/**
 * The timer class is used to get the current time and to provide callbacks.
 * A timer object calls its listeners back every so often based on the time
 * given.\n
 * All of the methods in this class are safe to call from multiple threads at
 * the same time, and can also be called from the TimerCallback as well, with
 * a few (some obvious) exceptions.
 */
//##ModelId=3B075380037B
class Timer : public Thread {
public:
  /**
   * Initalize a timer callback.  The first call to the callback will occur
   * after "rate" milliseconds, so this class is suitable for setting
   * timeouts for your operations.  Use the startTimer method to start this
   * timer.
   * @param callback A newly allocated object to perform callbacks on.
   * @param rate the callback rate in milliseconds.
   * @param destroy should the callback be destroyed when this object is
   *        destroyed (this is done with the delete operator)?
   */
  //##ModelId=3B0753820030
  Timer(TimerCallback* callback, int rate, bool destroy);

  /**
   * Destroys this timer object.  If the timer is running, it will be stopped,
   * and this destructor will block for at most the time of the callback rate
   * plus the time it takes for the callback function to complete.\n
   * If you chose when you created this object to call delete on the
   * TimerCallback, it will be done in the destructor.\n
   * The callback function itself CANNOT destroy its Timer since the callback
   * is actually using the Thread this class represents.  A TimerCallback
   * object could delete a Timer, but not from the same thread the callback
   * function is called in (see exnetperf's StatsDisplay for an example).
   */
  //##ModelId=3B0753820034
  virtual ~Timer();

  /**
   * Returns the current time from some arbitray point in the past.  This is
   * usually a very high precision timer.  Where possible, the rdtsc
   * instruction is used to get time on x86 platforms.  Anything else the
   * highest precision timer will be used.\n
   * Resolutions:\n
   * Windows/x86: (probably) 1 / CPU clock speed
   */
  //##ModelId=3B0753820036
  static Time getCurrentTime();

  /**
   * Returns the current time from the system clock.
   * The time returned is an absolute time,
   * usually relative to midnight, Jan 1, 1970.\n
   * Resolutions:\n
   * Windows: 10ms
   */
  //##ModelId=3B0753820065
  static Time getAbsoluteTime();

  /**
   * Starts the timer running and calling the callback.  If the timer has
   * already started, this call will have no effect.
   */
  //##ModelId=3B0753820067
  void startTimer();

  /**
   * Stops the timer and stops calling the callback.  The timer will likely
   * be called one more time because the timer will actually stop at the end
   * of its most recent cycle.  If you want to wait until the callback is
   * called for the last time, pass true into this function.  Then this
   * function will block for at most the time of the callback rate plus the
   * time it takes for the callback to finish.\n
   * This timer's callback can call this function, but obviously it must not
   * pass true to this function.\n
   * If a Timer is already stopped, this function will have no effect.
   */
  //##ModelId=3B0753820068
  void stopTimer(bool waitForEnd);

protected:
  /**
   * This is the thread that will perform the callbacks.
   */
  //##ModelId=3B0753820069
  void run();

private:
  /**
   * Next time the callbacks will be activated.
   */
  //##ModelId=3B07538103FB
  Time nextTime;

  /**
   * The callback rate in microseconds.  This is different from the accepted
   * parameter in the constructor of milliseconds.
   */
  //##ModelId=3B0753820029
  int callbackRate;

  //##ModelId=3B075382002B
  TimerCallback* listener;

  /**
   * Should listener be destroyed with this object?
   */
  //##ModelId=3B075382002F
  bool destroyListener;

  /**
   * Provides syncronization for some functions to make them thread safe.
   */
  //##ModelId=3C30E3FF029D
  Mutex sync;

};

}
#endif /* TIMER_H_INCLUDED_C517B9FE */



