#ifndef _SYNCHRONIZEDOBJECT_H_
#define _SYNCHRONIZEDOBJECT_H_

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

#include "Mutex.h"

namespace GNE {

/**
 * @ingroup threading
 *
 * A class meant to act as a base class for objects who might want an acquire/
 * release pair of functions tied to a mutex for object-level synchronization.
 *
 * You can inherit from this class publically to allow for external sources to
 * do locking.  Protected inheritance is possible to allow for implicit
 * locking internally, but likely would not be any more useful and declaring a
 * Mutex object.
 */
class SynchronizedObject {
public:
  /**
   * Synchronizes on this object.
   */
  void acquire() {
    lock.acquire();
  }

  /**
   * Releases the lock on this object.
   */
  void release() {
    lock.release();
  }

protected:
  Mutex lock;
};

/**
 * @ingroup threading
 *
 * Like LockMutex, but works for SynchronizedObject.
 *
 * \todo Consider abstracting the algorithm through a template.
 *
 * @see LockMutex
 */
class LockObject {
public:
  LockObject(SynchronizedObject& objToLock) : obj(objToLock) {
    obj.acquire();
  }

  ~LockObject() {
    obj.release();
  }

private:
  //LockObject cannot be copied or constructed defaultly.
  LockObject();
  LockObject(LockObject&);
  LockObject& operator= (LockObject& rhs);

  SynchronizedObject& obj;
};

/**
 * @ingroup threading
 *
 * Like LockMutexEx, but works for SynchronizedObject.
 *
 * @see LockMutexEx
 */
class LockObjectEx {
public:
  LockObjectEx(SynchronizedObject& objToLock) : obj(objToLock), released(false) {
    obj.acquire();
  }

  void release() {
    if ( !released ) {
      obj.release();
      released = true;
    }
  }

  ~LockObjectEx() {
    if ( !released )
      obj.release();
  }

private:
  //LockObjectEx cannot be copied or constructed defaultly.
  LockObjectEx();
  LockObjectEx(LockObjectEx&);
  LockObjectEx& operator= (LockObjectEx& rhs);

  SynchronizedObject& obj;

  bool released;
};

} //namespace GNE

#endif //_SYNCHRONIZEDOBJECT_H_