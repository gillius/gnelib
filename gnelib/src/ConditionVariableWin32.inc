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

#ifdef WIN32
#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/ConditionVariable.h"
#include "../include/gnelib/Mutex.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Timer.h"
#include <limits>
//There appears to be a max() macro defined that is messing things up in
//at least MSVC.NET, and we don't use a max() define in this source file.
#undef max

namespace GNE {

ConditionVariable::ConditionVariable()
: m_gone(0), m_blocked(0), m_waiting(0) {

  initBoostCode();
  ourMutex = true;
  mutex = new Mutex();
}

ConditionVariable::ConditionVariable(Mutex* m) {
  initBoostCode();
  ourMutex = false;
  mutex = m;
}

void ConditionVariable::initBoostCode() {
  m_gate = CreateSemaphore(0, 1, 1, 0);
  m_queue = CreateSemaphore(0, 0, std::numeric_limits<long>::max(), 0);
  m_mutex = CreateMutex(0, 0, 0);
  assert(m_gate && m_queue && m_mutex);
}

ConditionVariable::~ConditionVariable() {
  //Shutdown for boost code
  int res = 0;
  res = CloseHandle(m_gate);
  assert(res);
  res = CloseHandle(m_queue);
  assert(res);
  res = CloseHandle(m_mutex);
  assert(res);

  //Our shutdown code
  if (ourMutex)
    delete mutex;
}

void ConditionVariable::acquire() {
  mutex->acquire();
}

void ConditionVariable::release() {
  mutex->release();
}

void ConditionVariable::enter_wait() {
  int res = 0;
  res = WaitForSingleObject(m_gate, INFINITE);
  assert(res == WAIT_OBJECT_0);
  ++m_blocked;
  res = ReleaseSemaphore(m_gate, 1, 0);
  assert(res);
}

void ConditionVariable::wait() {
  enter_wait();
  release();

  int res = 0;
  res = WaitForSingleObject(m_queue, INFINITE);
  assert(res == WAIT_OBJECT_0);

  unsigned was_waiting=0;
  unsigned was_gone=0;

  res = WaitForSingleObject(m_mutex, INFINITE);
  assert(res == WAIT_OBJECT_0);
  was_waiting = m_waiting;
  was_gone = m_gone;
  if (was_waiting != 0)
  {
    if (--m_waiting == 0)
    {
      if (m_blocked != 0)
      {
        res = ReleaseSemaphore(m_gate, 1, 0); // open m_gate
        assert(res);
        was_waiting = 0;
      }
      else if (m_gone != 0)
        m_gone = 0;
    }
  }
  else if (++m_gone == (std::numeric_limits<unsigned>::max() / 2))
  {
    // timeout occured, normalize the m_gone count
    // this may occur if many calls to wait with a timeout are made and
    // no call to notify_* is made
    res = WaitForSingleObject(m_gate, INFINITE);
    assert(res == WAIT_OBJECT_0);
    m_blocked -= m_gone;
    res = ReleaseSemaphore(m_gate, 1, 0);
    assert(res);
    m_gone = 0;
  }
  res = ReleaseMutex(m_mutex);
  assert(res);

  if (was_waiting == 1)
  {
    for (/**/ ; was_gone; --was_gone)
    {
      // better now than spurious later
      res = WaitForSingleObject(m_queue, INFINITE);
      assert(res == WAIT_OBJECT_0);
    }
    res = ReleaseSemaphore(m_gate, 1, 0);
    assert(res);
  }

  acquire();
}

void ConditionVariable::timedWait(int ms) {
  enter_wait();
  release();

  unsigned int res = 0;
  res = WaitForSingleObject(m_queue, ms);
  assert(res != WAIT_FAILED && res != WAIT_ABANDONED);

  bool ret = (res == WAIT_OBJECT_0);

  unsigned was_waiting=0;
  unsigned was_gone=0;

  res = WaitForSingleObject(m_mutex, INFINITE);
  assert(res == WAIT_OBJECT_0);
  was_waiting = m_waiting;
  was_gone = m_gone;
  if (was_waiting != 0)
  {
    if (!ret) // timeout
    {
      if (m_blocked != 0)
        --m_blocked;
      else
        ++m_gone; // count spurious wakeups
    }
    if (--m_waiting == 0)
    {
      if (m_blocked != 0)
      {
        res = ReleaseSemaphore(m_gate, 1, 0); // open m_gate
        assert(res);
        was_waiting = 0;
      }
      else if (m_gone != 0)
        m_gone = 0;
    }
  }
  else if (++m_gone == (std::numeric_limits<unsigned>::max() / 2))
  {
    // timeout occured, normalize the m_gone count
    // this may occur if many calls to wait with a timeout are made and
    // no call to notify_* is made
    res = WaitForSingleObject(m_gate, INFINITE);
    assert(res == WAIT_OBJECT_0);
    m_blocked -= m_gone;
    res = ReleaseSemaphore(m_gate, 1, 0);
    assert(res);
    m_gone = 0;
  }
  res = ReleaseMutex(m_mutex);
  assert(res);

  if (was_waiting == 1)
  {
    for (/**/ ; was_gone; --was_gone)
    {
      // better now than spurious later
      res = WaitForSingleObject(m_queue, INFINITE);
      assert(res ==  WAIT_OBJECT_0);
    }
    res = ReleaseSemaphore(m_gate, 1, 0);
    assert(res);
  }

  acquire();
}

void ConditionVariable::timedWait(const Time& until) {
  Time t = until - Timer::getAbsoluteTime();
  int microSec = t.getTotaluSec();
  if (microSec <= 0) {
    //Return immediately because the time has already passed.
    return;
  } else {
    timedWait(microSec / 1000);
  }
}

void ConditionVariable::signal() {
  unsigned signals = 0;

  int res = 0;
  res = WaitForSingleObject(m_mutex, INFINITE);
  assert(res == WAIT_OBJECT_0);

  if (m_waiting != 0) // the m_gate is already closed
  {
    if (m_blocked == 0)
    {
      res = ReleaseMutex(m_mutex);
      assert(res);
      return;
    }

    ++m_waiting;
    --m_blocked;
    signals = 1;
  }
  else
  {
    res = WaitForSingleObject(m_gate, INFINITE);
    assert(res == WAIT_OBJECT_0);
    if (m_blocked > m_gone)
    {
      if (m_gone != 0)
      {
        m_blocked -= m_gone;
        m_gone = 0;
      }
      signals = m_waiting = 1;
      --m_blocked;
    }
    else
    {
      res = ReleaseSemaphore(m_gate, 1, 0);
      assert(res);
    }
  }

  res = ReleaseMutex(m_mutex);
  assert(res);

  if (signals)
  {
    res = ReleaseSemaphore(m_queue, signals, 0);
    assert(res);
  }
}

void ConditionVariable::broadcast() {
  unsigned signals = 0;

  int res = 0;
  res = WaitForSingleObject(m_mutex, INFINITE);
  assert(res == WAIT_OBJECT_0);

  if (m_waiting != 0) // the m_gate is already closed
  {
    if (m_blocked == 0)
    {
      res = ReleaseMutex(m_mutex);
      assert(res);
      return;
    }

    m_waiting += (signals = m_blocked);
    m_blocked = 0;
  }
  else
  {
    res = WaitForSingleObject(m_gate, INFINITE);
    assert(res == WAIT_OBJECT_0);
    if (m_blocked > m_gone)
    {
      if (m_gone != 0)
      {
        m_blocked -= m_gone;
        m_gone = 0;
      }
      signals = m_waiting = m_blocked;
      m_blocked = 0;
    }
    else
    {
      res = ReleaseSemaphore(m_gate, 1, 0);
      assert(res);
    }

    res = ReleaseMutex(m_mutex);
    assert(res);

    if (signals)
    {
      res = ReleaseSemaphore(m_queue, signals, 0);
      assert(res);
    }
  }
}

}
#endif
