#ifndef COUNTER_H_INCLUDED_C517BD95
#define COUNTER_H_INCLUDED_C517BD95

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
#include "TimerCallback.h"
#include "Mutex.h"

namespace GNE {

/**
 * An implementation of a TimerCallback that provides a simple counter.
 */
//##ModelId=3B07538002DB
class Counter : public TimerCallback {
public:
  /**
   * Initalize this Counter.  Pass this Counter into a Timer class to use it.
   * @see TimerCallback
   */
  //##ModelId=3B07538100EB
  Counter();

  //##ModelId=3B07538100EC
  virtual ~Counter();

  /**
   * The callback function that the Timer calls.  You should not call this
   * function directly.
   */
  //##ModelId=3B07538100EE
  void timerCallback();

  /**
   * Returns the current count of this Counter.  This function is syncronized
   * with its own mutex so it is safe to call from multiple threads.
   */
  //##ModelId=3B0753810118
  int getCount();

  /**
   * Sets the current count to newCount.  This function is syncronized
   * with its own mutex so it is safe to call from multiple threads.
   * @param newCount the new value for the Counter
   */
  //##ModelId=3B0753810119
  void setCount(int newCount);


private:
  //##ModelId=3B07538100E5
  int count;

  //##ModelId=3B07538100E7
  Mutex countSync;

};

}
#endif /* COUNTER_H_INCLUDED_C517BD95 */


