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

//NOTES ON THREADS:
// All threads should be placed on the heap (new operator)
// All threads must either be detached or joined at some point.
// Detached threads will delete themselves when they die.
// When you detach a thread, you should never access that thread
//   object again.
// If you need to access a thread object later, keep the pointer
//   to it so you can access/join it later.  When you join, access
//   whatever data you want from it then delete it.

#include "../../src/gnelib.h"

#include <sys/timeb.h>
#include <pthread.h>

class HelloWorld : public Thread {
public:
  HelloWorld(std::string myName) : Thread(myName, DEF_PRI) {
    Console::mprintf("%s is born\n", myName.c_str());
  }
  virtual ~HelloWorld() {
    Console::mprintf("%s dies.\n", getName().c_str());
  }

  void run() {
    Console::mprintf("Hello World!  My name is %s.\n Id: %i %i\n Ref: %x %x\n",
      getName().c_str(), getID(), pthread_self(), Thread::currentThread(), this);
  }
};

int main(int argc, char* argv[]) {
  GNE::init(GNE::NO_NET, atexit);
  Console::init(atexit);
  Console::setTitle("GNE Threads Example");

  HelloWorld* bob = new HelloWorld("Bob");
  bob->start();      //Tells bob to start his job.
  bob->detach(true); //we don't want to see bob again.

  HelloWorld* sally = new HelloWorld("Sally");
  sally->start();

  HelloWorld* joe = new HelloWorld("Joe");
  joe->start();

  assert(sally->isRunning()); //Sally should be running.  Note this is true
                              //because sally->start() was called, and not
                              //because the thread has ACTUALLY started
                              //executing.
  Console::mprintf("Waiting for Sally to end.\n");
  sally->join(); //we must either join or detach every thread.
  Console::mprintf("Sally ended.  Killing Sally.\n");
  assert(sally->hasEnded());
  delete sally;  //in the join case, we must delete the thread after joining.

  Console::mprintf("Sally died.  Now waiting for Joe to end.\n");
  while (!joe->hasEnded()) {}
  Console::mprintf("Joe has ended.  Detaching Joe.\n");
  joe->detach(true); //But even if we detach after a thread ends it will
                     //still destroy itself.

  //Sleep, measuring our sleep time two ways
  clock_t start = clock();
  Time lastTime = Timer::getCurrentTime();

  Console::mprintf("Sleeping for 2459 milliseconds.\n");
  Thread::sleep(2459);

  Time diffTime = Timer::getCurrentTime();
  clock_t finish = clock();
  float sysNapTime = (float)(finish - start) / CLOCKS_PER_SEC;
  diffTime = diffTime - lastTime;

  Console::mprintf("System reports sleeping time of %f\n", sysNapTime);
  Console::mprintf("GNE timers report sleeping time of %i microseconds (us)\n", diffTime.getTotaluSec());

  //At this point, all threads that are running will be terminated.  When
  //main exits, everything goes.  Use join if you want to guarantee your
  //threads have exited
  return 0;
}