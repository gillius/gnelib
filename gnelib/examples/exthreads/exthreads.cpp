//NOTES ON THRADS:
// All threads should be placed on the heap (new operator)
// All threads must either be detached or joined at some point.
// Detached threads will delete themselves when they die.
// When you detach a thread, you should never access that thread
//   object again.
// If you need to access a thread object later, keep the pointer
//   to it so you can access/join it later.  When you join, access
//   whatever data you want from it then delete it.

#include "../../src/gnelib.h"
#include <stdio.h>
#include <stdarg.h>

#include <sys/timeb.h>
#include <pthread.h>

/**
 * Function to sync on printf.
 */
void mprintf(char *format, ...) {
  static Mutex outSync;
  va_list arg;
  
  va_start(arg, format);
  outSync.acquire();
  vfprintf(stderr, format, arg);
  outSync.release();
  va_end(arg);
}

class HelloWorld : public Thread {
public:
  HelloWorld(std::string myName) : Thread(myName, DEF_PRI) {
    mprintf("%s is born\n", myName.c_str());
  }
  virtual ~HelloWorld() {
    mprintf("%s dies.\n", getName().c_str());
  }

  void run() {
    mprintf("Hello World!  My name is %s.\n Id: %i %i\n Ref: %x %x\n",
      getName().c_str(), getID(), pthread_self(), Thread::currentThread(), this);
  }
};

int main(int argc, char* argv[]) {
  GNE::init(NL_IP);
  HelloWorld* bob = new HelloWorld("Bob");
  bob->start();  //Tells bob to start his job.
  bob->detach(); //we don't want to see bob again.

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
  assert(sally->hasEnded());
  delete sally;  //in the join case, we must delete the thread after joining.

  mprintf("Sally died.  Now waiting for Joe to end.\n");
  while (!joe->hasEnded()) {}
  mprintf("Joe has ended.  Detaching Joe.\n");
  joe->detach(); //But even if we detach after a thread ends it will still
                 //destroy itself.

  clock_t start = clock();
  mprintf("Sleeping for 2459 milliseconds.\n");
  Thread::sleep(2459);
  clock_t finish = clock();
  float napTime = (float)(finish - start) / CLOCKS_PER_SEC;

  mprintf("Goodbye.  A nice %f second nap that was.\n", napTime);

  GNE::exit(0);
  return 0;
}
