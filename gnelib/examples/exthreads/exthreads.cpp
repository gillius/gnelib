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
#include <pthread.h>

using namespace GNE;
using namespace GNE::Console;

class HelloWorld : public Thread {
public:
  HelloWorld(std::string myName) : Thread(myName) {
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
    mprintf("Hello World!  My name is %s.\n Id: %i %i\n Ref: %x %x\n",
      getName().c_str(), getID(), pthread_self(), Thread::currentThread(), this);
    //}
  }
};

int main(int argc, char* argv[]) {
  initGNE(NO_NET, atexit);
  initConsole(atexit);
  setTitle("GNE Threads Example");

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

  //Sleep, measuring our sleep time two ways
  clock_t start = clock();
  Time lastTime = Timer::getCurrentTime();

  mprintf("Sleeping for 2459 milliseconds.\n");
  Thread::sleep(2459);

  Time diffTime = Timer::getCurrentTime();
  clock_t finish = clock();
  float sysNapTime = (float)(finish - start) / CLOCKS_PER_SEC;
  diffTime = diffTime - lastTime;

  mprintf("System reports sleeping time of %f\n", sysNapTime);
  mprintf("GNE timers report sleeping time of %i microseconds (us)\n", diffTime.getTotaluSec());
  mprintf("Press a key to continue.\n");
  getch();

  //At this point, all threads that are running will be terminated.  When
  //main exits, everything goes.  Use join if you want to guarantee your
  //threads have exited
  return 0;
}


