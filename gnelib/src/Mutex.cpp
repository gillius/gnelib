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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/Mutex.h"

namespace GNE {

//##ModelId=3B075381014B
Mutex::Mutex() {
#ifdef WIN32
  InitializeCriticalSection(&mutex);
#else
  pthread_mutexattr_t attr;
  valassert(pthread_mutexattr_init(&attr), 0);
  valassert(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE), 0);
  valassert(pthread_mutex_init( &mutex, &attr ), 0);
  valassert(pthread_mutexattr_destroy(&attr), 0);
#endif
}

//##ModelId=3B075381014C
Mutex::~Mutex() {
#ifdef WIN32
  DeleteCriticalSection(&mutex);
#else
  valassert(pthread_mutex_destroy( &mutex ), 0);
#endif
}

//##ModelId=3B075381014E
void Mutex::acquire() {
#ifdef WIN32
  EnterCriticalSection(&mutex);
#else
  valassert(pthread_mutex_lock( &mutex ), 0);
#endif
}

//##ModelId=3B075381014F
void Mutex::release() {
#ifdef WIN32
  LeaveCriticalSection(&mutex);
#else
  valassert(pthread_mutex_unlock( &mutex ), 0);
#endif
}

}





