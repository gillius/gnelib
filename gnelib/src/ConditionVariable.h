#ifndef CONDITIONVARIABLE_H_INCLUDED_C51DFF03
#define CONDITIONVARIABLE_H_INCLUDED_C51DFF03

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
class Mutex;

/**
 * A class for a conditional variable.
 * This class is a wrapper for pthreads, so please read man pages or other
 * documentation for the equivalent pthreads functions if you need to
 * understand the specifics on the semantics of these functions.
 */
//##ModelId=3AE20D770230
class ConditionVariable {
public:
  /**
   * Initalizes this class creating a new mutex.
   */
  //##ModelId=3AE20E490140
  ConditionVariable();

  /**
   * Initalizes this class, with another mutex that the caller is
   * responsible for allocating and destroying.
   * @param m the mutex to be associated with this conditional variable.
   *          this mutex will not be deallocated.
   */
  //##ModelId=3AE50F1A0398
  ConditionVariable(Mutex* m);

  //##ModelId=3AE20E880104
  virtual ~ConditionVariable();

	/**
   * Locks the associated mutex for this condition variable.
   * @see Mutex#acquire()
   */
  //##ModelId=3AE50F1B00FA
  void acquire();

  /**
   * Releases the associated mutex.
   * @see Mutex#release()
   */
  //##ModelId=3AE50F1B0168
  void release();

  /**
   * This method will block until the thread is woken up by a call to
   * signal or broadcast.  The associated mutex must be locked when wait()
   * is called.  wait() will unlock the mutex and block until woken up at
   * which point it will re-acquire the mutex.
   */
  //##ModelId=3AE20E5F02DA
  void wait();

  /**
   * This method works identical to wait, except the thread will wake up
   * anyways if not woken up before the timeout.  The mutex will still be
   * acquired.
   * @param timeout the time to wait in milliseconds
   */
  //##ModelId=3AEEF7660032
  void timedWait(int ms);

  /**
   * Wakes up at least one thread waiting on this CV.  Which thread wakes up
   * is implementation dependant.
   */
  //##ModelId=3AE20E7200B4
  void signal();

  /**
   * Wakes up all threads waiting on this CV.
   */
  //##ModelId=3AE20E7A00FA
  void broadcast();

private:
  //##ModelId=3AE20D83001E
  pthread_cond_t cond;

  /**
   * is the mutex pointer we have our mutex we should delete?
   */
  //##ModelId=3AE50F1A0280
  bool ourMutex;

  /**
   * The mutex associated with this Conditional Variable
   */
  //##ModelId=3AE2139802D1
  Mutex* mutex;
};

#endif /* CONDITIONVARIABLE_H_INCLUDED_C51DFF03 */


