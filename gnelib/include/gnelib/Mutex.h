#ifndef MUTEX_H_INCLUDED_C51DB1E6
#define MUTEX_H_INCLUDED_C51DB1E6

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


namespace GNE {

/**
 * The mutex class models a simple mutex to provide mutual exclusion to a
 * resource shared by threads.
 *
 * This version of a mutex is recursive.  This means that if a thread already
 * owns the mutex, then a second lock will not deadlock.  You need to unlock
 * the mutex the same number of times it was locked to unlock it.
 */
class Mutex {
public:
  /**
   * Initalizes this mutex.
   */
  Mutex();

  virtual ~Mutex();

  /**
   * Acquire the lock on a mutex, blocking if necessary.
   * <ol><li>If the mutex is unlocked, lock it, and proceed.</li>
   *     <li>If the mutex is locked, wait for it to become unlocked, then go
   *         to step 1.</li></ol>
   */
  void acquire();

  /**
   * Releases a locked mutex.
   */
  void release();

private:
  struct MutexData;
  MutexData* data;

  friend class ConditionVariable;
};

/**
 * A small helper class for Mutex which locks the mutex in its ctor and
 * unlocks it in its destructor.  This is handy if a largish function has
 * multiple exit points espically if it throws exceptions.  This class makes
 * sure that once you lock the mutex it will be unlocked when you leave the
 * scope.
 *
 * A LockMutex instance may not be copied, and you must specify a mutex to
 * lock in its constructor.
 *
 * This class might be expanded in the future to allow it to be released
 * early, and provide additional features, but its core functionality will
 * remain the same.
 */
class LockMutex {
public:
  LockMutex(Mutex& mutexToLock) : mutex(mutexToLock) {
    mutex.acquire();
  }

  ~LockMutex() {
    mutex.release();
  }
private:
  //LockMutex cannot be copied or constructed defaultly.
  LockMutex();
  LockMutex(LockMutex&);
  LockMutex& operator= (LockMutex& rhs);

  Mutex& mutex;
};

}
#endif /* MUTEX_H_INCLUDED_C51DB1E6 */
