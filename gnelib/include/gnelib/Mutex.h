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
 * @ingroup threading
 *
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
  //These are used by Thread::currentThread to lock and unlock mapsync,
  //because currentThread is used for debugging in the main methods.
  friend class Thread;
  void vanillaAcquire();
  void vanillaRelease();

  struct MutexData;
  MutexData* data;

  friend class ConditionVariable;
};

/**
 * @ingroup threading
 *
 * A small helper class for Mutex which locks the mutex in its ctor and
 * unlocks it in its destructor.  This is handy if a largish function has
 * multiple exit points espically if it throws exceptions.  This class makes
 * sure that once you lock the mutex it will be unlocked when you leave the
 * scope.
 *
 * A LockMutex instance may not be copied, and you must specify a mutex to
 * lock in its constructor.
 *
 * @see LockMutexEx
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

/**
 * @ingroup threading
 *
 * Works exactly like LockMutex but allows early release.  Two separate classes
 * were provided because on many compilers LockMutex has absolutely 0 overhead
 * while when trying to provide early release, there may be some overhead.
 *
 * Since those cases are rare, the ex version can be used when early release
 * is needed.
 *
 * @see LockMutex
 */
class LockMutexEx {
public:
  LockMutexEx(Mutex& mutexToLock) : mutex(mutexToLock), released(false) {
    mutex.acquire();
  }

  void release() {
    if ( !released ) {
      mutex.release();
      released = true;
    }
  }

  ~LockMutexEx() {
    if ( !released )
      mutex.release();
  }
private:
  //LockMutexEx cannot be copied or constructed defaultly.
  LockMutexEx();
  LockMutexEx(LockMutexEx&);
  LockMutexEx& operator= (LockMutexEx& rhs);

  Mutex& mutex;

  bool released;
};

}
#endif /* MUTEX_H_INCLUDED_C51DB1E6 */
