#ifndef MUTEX_H_INCLUDED_C51DB1E6
#define MUTEX_H_INCLUDED_C51DB1E6

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

/**
 * The mutex class models a simple mutex.
 * This class is a wrapper for pthreads, so please read man pages or other
 * documentation for the equivalent pthreads functions if you need to
 * understand the specifics on the semantics of these functions.
 */
//##ModelId=3AE208590280
class Mutex {
public:
  /**
   * Initalizes this mutex.
   */
  //##ModelId=3AE20A040384
  Mutex();

  //##ModelId=3AE20A040385
  virtual ~Mutex();

	/**
   * Acquire the lock on a mutex, blocking if necessary.
   * 1. If the mutex is unlocked, lock it, and proceed.\n
	 * 2. If the mutex is locked, wait for it to become unlocked,
   *    then go to step 1.
   */
  //##ModelId=3AE20A070398
  void acquire();

  /**
   * Releases a locked mutex.
   */
  //##ModelId=3AE20A0D035C
  void release();

private:
  //##ModelId=3AE2097B03CA
  pthread_mutex_t mutex;

  friend class ConditionVariable;
};

#endif /* MUTEX_H_INCLUDED_C51DB1E6 */


