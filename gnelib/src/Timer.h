#ifndef TIMER_H_INCLUDED_C517B9FE
#define TIMER_H_INCLUDED_C517B9FE

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
#include "Time.h"

namespace GNE {
class TimerCallback;

/**
 * The timer class is used to get the current time and to provide callbacks.
 * A timer object calls its listeners back every so often based on the time
 * given.
 */
//##ModelId=3B075380037B
class Timer : public Thread {
public:
  /**
   * Initalize a timer callback.
   * @param callback A newly allocated object to perform callbacks on.
   * @param rate the callback rate in milliseconds.
   * @param destroy should the callback be destroyed when this object is
   *        destroyed?
   */
  //##ModelId=3B0753820030
  Timer(TimerCallback* callback, int rate, bool destroy);

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
   * Starts the timer running and calling the callback.
   */
  //##ModelId=3B0753820067
  void startTimer();

  /**
   * Stops the timer and stops calling the callback.  This function blocks
   * until the timer is stopped, which may last as long as the timer's
   * callback.
   */
  //##ModelId=3B0753820068
  void stopTimer();

  /**
   * Don't call this function.  This is the thread that will perform the
   * callbacks.
   */
  //##ModelId=3B0753820069
  void run();

private:
  /**
   * Next time the callbacks will be activated.
   */
  //##ModelId=3B07538103FB
  Time nextTime;

  //##ModelId=3B0753820029
  int callbackRate;

  //##ModelId=3B075382002B
  TimerCallback* listener;

  /**
   * Should listener be destroyed with this object?
   */
  //##ModelId=3B075382002F
  bool destroyListener;

};

}
#endif /* TIMER_H_INCLUDED_C517B9FE */