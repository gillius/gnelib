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

#include "../../src/gnelib.h"

using namespace GNE;
using namespace Console;

class TimeClass : public TimerCallback {
public:
  TimeClass(int x2, int y2, std::string ourName)
    : lastTime(Timer::getCurrentTime()), callNum(0), x(x2), y(y2), name(ourName) {}

  virtual ~TimeClass() {}

  void timerCallback() {
    Time currTime = Timer::getCurrentTime();
    Time diffTime = currTime - lastTime;
    lastTime = currTime;
    mlprintf(x, y, "(%i)Hello, I'm %s! change: %ius", callNum, name.c_str(), diffTime.getTotaluSec());
    callNum++;
  }

private:
  Time lastTime;
  int callNum;
  int x, y;
  std::string name;
};

class CounterWatcher : public TimerCallback {
public:
  CounterWatcher(int x2, int y2, int counterRate)
    : x(x2), y(y2), timer(&counter, counterRate, false) {
    //false == don't let Timer kill the callback (counter)
    timer.startTimer();
  }

  virtual ~CounterWatcher() {
    timer.stopTimer();
  }

  void timerCallback() {
    mlprintf(x, y, "Count: %i", counter.getCount());
  }

private:
  int x, y;
  Counter counter;
  Timer timer;
};
  
int main() {
  initGNE(NO_NET, atexit);
  initConsole(atexit);
  setTitle("GNE Timers Example");

  //Doing some tests on Time class
  mprintf("Time class tests:\n");
  Time t(0, 1000000);
  mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t += 1000001; //add a little over one second
  mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t.setSec(5);
  mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t = t + Time(3, 5500000);
  mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  mprintf("Timer class tests, press a key to quit:\n");

  //Create the timers, and pass true to allow our timer objects to
  //also destroy their callbacks when they die.
  Timer t1(new TimeClass(3, 8, "Bob"), 1000, true);
  Timer t2(new TimeClass(5, 10, "Sally"), 1250, true);
  Timer t3(new TimeClass(1, 12, "Joe"), 200, true);
  Timer t4(new CounterWatcher(40, 3, 10), 500, true);

  //Start the timers
  t1.startTimer();
  assert(t1.isRunning());
  t2.startTimer();
  assert(t2.isRunning());
  t3.startTimer();
  assert(t3.isRunning());
  t4.startTimer();
  assert(t4.isRunning());

  getch(); //Wait for a keypress.

  mlprintf(0, 14, "Shutting down timers, please wait...");
  t1.stopTimer();
  t2.stopTimer();
  t3.stopTimer();
  t4.stopTimer();

  return 0;
}