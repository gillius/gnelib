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

#include "Mutex.h"
#include <string>

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
class Thread {
public:
  /**
   * Creates a new thread, ready to run but not yet running.
   * @param name2 the name of this thread
   * @param priority2 the priority this thread has
   */
  Thread(std::string name2 = DEF_NAME, int priority2 = DEF_PRI);

  virtual ~Thread();

  /**
   * Returns the Thread object that represents this calling thread.
   * @return the currently running thread, or NULL if no Thread object
   *         resembles this thread.
   */
  static Thread* currentThread();

  /**
   * The currently running thread sleeps for the time given in milliseconds.
   */
  static void sleep(int ms);

  /**
   * Makes a request to the operating system to give up the remainder of this
   * thread's timeslice to another appropriate thread.  This function does
   * nothing if the operating system does not support the call.  Note that
   * even if the OS supports the call, it may choose not to end your
   * timeslice.
   *
   * There are certain few cases where yielding may improve performance or
   * response, but most of the time you are likely wanting to use a
   * ConditionVariable to wait for an event or a Timer.  You should never use
   * yielding as mitigation for a busy-wait, because you will still use 100%
   * CPU and essentially you will perform a busy-wait and just lower the
   * scheduling priority of your thread/process.
   */
  static void yield();

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
  static bool waitForAllThreads(int ms);

  /**
   * Returns the name of this thread.
   */
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
  virtual void shutDown();

  /**
   * A blocking call that returns when this thread terminates, or returns
   * immediately if the thread has terminated.  A thread must always either
   * be joined or detached if system resources are to be recovered.  A
   * thread may not be joined after being detached.
   * @see detach
   */
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
  void detach(bool delThis);

  /**
   * Determine the running state of the thread.
   */
  bool isRunning() const;

  /**
   * Starts this thread running.  The thread will be marked as running before
   * this function completes.
   */
  virtual void start();
  
  /**
   * Returns the priority of this class.
   */
  int getPriority() const;

  /**
   * The default priority of a thread.
   */
  static const int DEF_PRI;

  /**
   * The default name for a thread.
   */
  static const std::string DEF_NAME;

  /**
   * A lowered priority for a thread.  Might be good for background
   * operations like loading and saving files or something so it does not
   * interfer with the user interface (be it a GUI or a game or whatever).
   */
  static const int LOW_PRI;

  /**
   * A lower priority for a thread than LOW_PRI.
   */
  static const int LOWER_PRI;

  /**
   * A "boosted priority" for a thread.  This is optimized to give some extra
   * priority for the network threads to reduce network latency in favor of a
   * little in-game fps.
   */
  static const int HIGH_PRI;

  /**
   * Even higher priority thread than HIGH_PRI.  Used typically for the timer
   * threads.
   */
  static const int HIGHER_PRI;

protected:
  /**
   * This variable is set to true when this thread should exit.  Users that
   * implement a Thread class must respond to this if the thread wants to
   * wait in an infinite loop.  Threads that will eventually stop can respond
   * only optionally to this, but if they last for a long time, shutting down
   * all threads may take awhile.  Responding to this flag usually takes
   * nothing more than a while (!shutdown) {} loop.
   */
  volatile bool shutdown;

  /**
   * This function is the starting point for this thread.
   * Derive your own class from Thread, and implement this function.
   * When this function returns, the thread ends.
   */
  virtual void run() = 0;

private:
  struct ThreadIDData;
  ThreadIDData* id;

#ifdef WIN32
  static unsigned __stdcall threadStart(void* thread);
#else
  static void* threadStart(void* thread);
#endif

  /**
   * This function is called internally and automatically after run ends and
   * is used for internal library cleanup.
   */
  void end();

  static void remove(Thread*);

  std::string name;

  bool running;

  bool deleteThis;

  int priority;

  Mutex sync;
};

}
#endif /* THREAD_H_INCLUDED_C51E3746 */
