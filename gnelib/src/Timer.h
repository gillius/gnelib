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
class TimerCallback;

/**
 * The timer class is used to get the current time and to provide callbacks.
 * A timer object calls its listener back every so often based on the time
 * given.
 */
//##ModelId=3AE8680C029E
class Timer : public Thread {
public:
  /**
   * Initalize a timer callback.
   * @param listener2 the TimerCallback instance to use for callbacks
   * @param rate the callback rate in milliseconds.
   */
  //##ModelId=3AE868370122
  Timer(TimerCallback& theListener, int rate);

  //##ModelId=3AE8686C00BE
  virtual ~Timer();

  /**
   * Returns the current time from some arbitrary point.  Resolution is only
   * guaranteed to the millisecond.  The time returned is an absolute time,
   * usually relative to midnight, Jan 1, 1970.
   */
  //##ModelId=3AE86872030C
  static Time getCurrentTime();

  /**
   * Called to set the current time to zero -- called when the library is
   * initalized.  This function can be called afterwards to reset the
   * counter back to zero but this should not be necessary.
   */
  //##ModelId=3AEB9AB4038E
  static void initTime();

  /**
   * Starts the timer running and calling the callback.
   */
  //##ModelId=3AEB9AB50050
  void startTimer();

  /**
   * Stops the timer and stops calling the callback.
   */
  //##ModelId=3AEB9AB500BE
  void stopTimer();

  /**
   * Don't call this function.  This is the thread that will perform the
   * callbacks.
   */
  //##ModelId=3AE868A30294
  void run();

  /**
   * Is this Timer running?
   */
  //##ModelId=3AED05E7029E
  bool isRunning();

private:
  /**
   * The starting time from initTime()
   */
  //##ModelId=3AEBA397028B
  static Time startTime;

  /**
   * Next time the callback will be activated.
   */
  //##ModelId=3AEBA39702C7
  Time nextTime;

  //##ModelId=3AEB9AB401E0
  bool running;

  //##ModelId=3AE8695C0028
  int callbackRate;

  //##ModelId=3AE86968026C
  TimerCallback& listener;

};



#endif /* TIMER_H_INCLUDED_C517B9FE */


