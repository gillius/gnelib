#ifndef THREAD_H_INCLUDED_C51E3746
#define THREAD_H_INCLUDED_C51E3746

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

#include "gneintern.h"
#include "Mutex.h"

namespace GNE {

/**
 * A class resembling a thread.
 * Derive your own classes from this class that implement the run function.
 * Execution begins in the run method.  All methods of this class are thread-
 * safe.
 *
 * NOTE: All threads must either be detached or joined.  If you choose to
 * detach a thread you may not access that Thread object again.  If you need
 * to access it at a later time, keep a pointer, then at a later time call
 * join on this pointer, then delete at some later time.
 *
 * \todo Priorities are not yet implemented.
 */
//##ModelId=3B075380034B
class Thread {
public:
  /**
   * Creates a new thread, ready to run but not yet running.
   * @param name2 the name of this thread
   * @param priority2 the priority this thread has
   */
  //##ModelId=3B0753810376
  Thread(std::string name2 = DEF_NAME, int priority2 = DEF_PRI);

  //##ModelId=3B0753810379
  virtual ~Thread();

  /**
   * Returns the Thread object that represents this calling thread.
   * @return the currently running thread, or NULL if no Thread object
   *         resembles this thread.
   */
  //##ModelId=3B075381037B
  static Thread* currentThread();

  /**
   * The currently running thread sleeps for the time given in milliseconds.
   */
  //##ModelId=3B075381037D
  static void sleep(int ms);

  /**
   * This method will wait for all threads that the user can control or see
   * to end.  All of the threads created implicitly have a deterministic end
   * so that when you destruct objects the threads associated with them are
   * deleted.  Therefore only threads that you have directly created and
   * detached (rather than joined) might still be running.  You can use this
   * static method to wait for and therefore verify their completion, since
   * when main exits, your threads will be terminated forcefully if they are
   * running.
   *
   * The implementation of this method is simple and therefore not intended
   * to be used to create a program where you create detached threads and
   * then use the main thread to sit in this function until the "real"
   * program completes.  It is meant solely as a method of definitvely
   * verifying the completion of detached threads, and waiting short times
   * for these threads to finish if needed.  If you need to wait a long time,
   * use join on the threads you've made, as this method is more efficient.
   *
   * @param ms the amount of time to wait for the threads before giving up.
   *           This is used so a stalled or crashed thread will still allow
   *           you to terminate the program.
   * @return true if all threads have completed, or false if there are still
   *              some threads running and the method timed out.
   */
  //##ModelId=3C885B3800E8
  static bool waitForAllThreads(int ms);

  /**
   * Returns the name of this thread.
   */
  //##ModelId=3B0753810380
  std::string getName() const;

  /**
   * Tells this thread to shutdown, if it is in an infinite loop.  You will
   * probably want to call join right after calling this to wait for the
   * shutdown to complete which is dependant on the thread you are shutting
   * down.
   *
   * This function is virtual if the thread needs any additional actions to
   * notify itself to shutdown, for example if it is waiting for some event on
   * a ConditionVariable.
   *
   * You will want to call this function from the override to make sure that
   * shutdown is set to true.
   *
   * This function is safe to call multiple times, but you cannot undo a
   * shutdown once it has been called.
   */
  //##ModelId=3B0C5919012C
  virtual void shutDown();

  /**
   * A blocking call that returns when this thread terminates, or returns
   * immediately if the thread has terminated.  A thread must always either
   * be joined or detached if system resources are to be recovered.  A
   * thread may not be joined after being detached.
   * @see detach
   */
  //##ModelId=3B0753810382
  void join();

  /**
   * Detaches a running thread.  A thread must always either
   * be joined or detached if system resources are to be recovered.  A
   * thread may not be joined after being detached.  After detaching a
   * thread, with delThis set to true, it
   * is undefined to access it any longer outside of its own actual thread.
   * In other words, at any moment after detach the Thread instance could be
   * be deleted because the thread terminated (or has already terminated).
   * But, of course a running thread could still access itself as it clearly
   * has not stopped and is still running.
   *
   * If you care about reading the data from a thread after its completion,
   * do not use detach and instead use join.  It is useful to do this if you
   * want to release OS thread resources
   * and still keep the class running (as in the Connection classes).
   *
   * Pass false if the the thread should not auto-destruct when it ends --
   * this is useful when you declared a thread on the stack, or you want to
   * poll for completion.
   *
   * @param delThis true if the thread should delete itself when it
   *                finishes executing.
   * @see join
   */
  //##ModelId=3B0753810383
  void detach(bool delThis);

  /**
   * Determine the running state of the thread.
   */
  //##ModelId=3B0753810388
  bool isRunning() const;

  /**
   * Starts this thread running.  The thread will be marked as running before
   * this function completes.
   */
  //##ModelId=3B07538103A5
  virtual void start();
  
  /**
   * Returns the priority of this class.
   */
  //##ModelId=3B07538103A6
  int getPriority() const;

  /**
   * The default priority of a thread.
   */
  //##ModelId=3B0753810334
  static const int DEF_PRI;

  /**
   * The default name for a thread.
   */
  //##ModelId=3AE1F1CA00DC
  static const std::string DEF_NAME;

  /**
   * A "boosted priority" for a thread.  This is optimized to give some extra
   * priority for the network threads to reduce network latency in favor of a
   * little in-game fps.
   */
  //##ModelId=3B0753810335
  static const int HIGH_PRI;

protected:
  /**
   * This variable is set to true when this thread should exit.  Users that
   * implement a Thread class must respond to this if the thread wants to
   * wait in an infinite loop.  Threads that will eventually stop can respond
   * only optionally to this, but if they last for a long time, shutting down
   * all threads may take awhile.  Responding to this flag usually takes
   * nothing more than a while (!shutdown) {} loop.
   */
  //##ModelId=3B0C591802EE
  volatile bool shutdown;

  /**
   * This function is the starting point for this thread.
   * Derive your own class from Thread, and implement this function.
   * When this function returns, the thread ends.
   */
  //##ModelId=3B0753810385
  virtual void run() = 0;

private:
#ifdef WIN32
  typedef DWORD ID;
  typedef unsigned RETCODE;
#define THREAD_CALLTYPE __stdcall
#else
  typedef pthread_t ID;
  typedef void* RETCODE;
#define THREAD_CALLTYPE
#endif

  /**
   * Internal Thread function for the pthread_start callback to start a new
   * thread and call run().
   */
  //##ModelId=3BB805C60186
  static RETCODE THREAD_CALLTYPE threadStart(void* thread);

  /**
   * This function is called internally and automatically after run ends and
   * is used for internal library cleanup.
   */
  //##ModelId=3B0753810387
  void end();

  /**
   * This is an internal-only function called by the underlying pthreads
   * functions -- do not call.
   */
  //##ModelId=3B07538103AA
  static void remove(Thread*);

  //##ModelId=3AE11D30014A
  std::string name;

  //##ModelId=3AE11D540136
  ID thread_id;

#ifdef WIN32
  HANDLE hThread;
#endif

  //##ModelId=3B0753810336
  bool running;

  //##ModelId=3B0753810338
  bool deleteThis;

  //##ModelId=3AE11D5F023A
  static std::map<ID, Thread*> threads;

  /**
   * Mutex for syncronizing threads
   */
  //##ModelId=3BB805C6014B
  static Mutex mapSync;

  //##ModelId=3B0753810339
  int priority;

  //##ModelId=3B075381033B
  Mutex sync;
};

}
#endif /* THREAD_H_INCLUDED_C51E3746 */


