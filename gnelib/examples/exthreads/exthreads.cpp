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

//NOTES ON THREADS:
// All threads should be placed on the heap (new operator)
// All threads must either be detached or joined at some point.
// Detached threads will delete themselves when they die.
// When you detach a thread, you should never access that thread
//   object again.
// If you need to access a thread object later, keep the pointer
//   to it so you can access/join it later.  When you join, access
//   whatever data you want from it then delete it.

#include "../../include/gnelib.h"

#include <sys/timeb.h>

using namespace GNE;
using namespace GNE::Console;

class HelloWorld : public Thread {
public:
  HelloWorld(std::string myName, Mutex& mutexToTest)
    : Thread(myName), testMutex(mutexToTest) {
    mprintf("%s is born\n", myName.c_str());
  }
  virtual ~HelloWorld() {
    mprintf("%s dies.\n", getName().c_str());
  }

protected:
  void run() {
    //Note that if we were sitting in an infinite loop, we would have to
    //respond to the shutdown variable, as shown in comments.  The commented
    //code would be the proper code if we wanted to repeat an action forever.
    //You would call shutDown then join on this thread to stop it in this
    //case.
    //while (!shutdown) {
    //  do_stuff();
    //}

    //Test the mutex we were given.  In normal operation the acquire should
    //not deadlock.  If it does there was a problem with unlocking the mutex
    //in the main thread.
    //The sleeps are to give the other threads a chance to go.  We should
    //never see two threads given the same mutex intertwine their prints.
    testMutex.acquire();

    mprintf("Hello World!  My name is %s.\n", getName().c_str());
    Thread::sleep(50);

    mprintf(" %s Ref: %x %x\n", getName().c_str(), Thread::currentThread(), this);

    assert(Thread::currentThread() == this);

    testMutex.release();
  }

private:
  //We try to acquire this mutex just to test it to see if it was really
  //unlocked in the main thread.
  Mutex& testMutex;
};

class WaitingThread : public Thread {
protected:
  void run() {
    sleep(1000);
  }
};

int main(int argc, char* argv[]) {
  initGNE(NO_NET, atexit);
  initConsole(atexit);
  setTitle("GNE Threads Example");

  mprintf("Testing to see if recursive locks work:");
  Mutex test;
  test.acquire();
  test.acquire();
  test.release();
  test.release();
  mprintf(" success.\n");

  Mutex test2;
  mprintf("Testing recursive locks with LockMutex:");
  try {
    //The LockMutex class locks the mutex when it is created.  When it is
    //destroyed the mutex is unlocked, and this is useful if a function has
    //mutliple return statements or throws exceptions.  You would never want
    //to use them as they are here (nested).  Only one LockMutex is ever
    //needed.
    LockMutex lock(test2);
    LockMutex lock2(test2);

    //When we throw an exception, variables in this block like lock and lock2
    //will be destroyed, and when a LockMutex is destroyed it unlocked the
    //mutex that it locked when it was created.
    throw 0;
  } catch (...) {
  }
  mprintf(" success.\n");

  //We make the HelloWorld threads, giving them some mutexes to test.  If two
  //threads are using the same mutex, they can't go at the same time.  So all
  //of Sally's statements should come before or after all of Joe's.  Bob uses
  //a different mutex so his statements will be mixed in with Sally's and
  //Joe's.
  HelloWorld* bob = new HelloWorld("Bob", test);
  bob->start();      //Tells bob to start his job.
  bob->detach(true); //we don't want to see bob again.

  HelloWorld* sally = new HelloWorld("Sally", test2);
  sally->start();

  HelloWorld* joe = new HelloWorld("Joe", test2);
  joe->start();

  assert(sally->isRunning()); //Sally should be running.  Note this is true
                              //because sally->start() was called, and not
                              //because the thread has ACTUALLY started
                              //executing by the OS having given it any CPU
                              //time.
  mprintf("Waiting for Sally to end.\n");
  sally->join(); //we must either join or detach every thread.
  mprintf("Sally ended.  Killing Sally.\n");
  assert(!sally->isRunning());
  delete sally;  //in the join case, we must delete the thread after joining.

  mprintf("Sally died.  Now waiting for Joe to end.\n");
  while (joe->isRunning()) {}
  mprintf("Joe has ended.  Detaching Joe.\n");
  joe->detach(true); //But even if we detach after a thread ends it will
                     //still destroy itself.

  //Sleep, measuring our sleep time
  Time lastTime = Timer::getCurrentTime();

  mprintf("Sleeping for 2459 milliseconds.\n");
  Thread::sleep(2459);

  Time diffTime = Timer::getCurrentTime();
  diffTime = diffTime - lastTime;

  mprintf("GNE timers report sleeping time of %i microseconds (us)\n", diffTime.getTotaluSec());
  
  mprintf("Now testing Thread::waitForAllThreads.\n");
  WaitingThread waiter;
  waiter.start();
  waiter.detach(false);
  //this should timeout as the waiter thread waits for 1000 ms.
  bool timeout = Thread::waitForAllThreads(150);
  if (timeout)
    mprintf("Thread::waitForAllThreads timed out properly.\n");
  else
    mprintf("Thread::waitForAllThreads method failed.\n");

  //The waiter thread should definitely finish during this call.
  timeout = Thread::waitForAllThreads(INT_MAX);
  if (timeout)
    mprintf("Thread::waitForAllThreads method failed.\n");
  else
    mprintf("Thread::waitForAllThreads 2nd call completed successfully.\n");
  
  mprintf("Press a key to continue.\n");
  getch();

  //At this point, all threads that are running will be terminated.  When
  //main exits, everything goes.  Use join if you want to guarantee your
  //threads have exited, or use waitForAllThreads to definitively verify that
  //detached threads have ended.
  return 0;
}


