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

class TimeClass : public TimerCallback {
public:
  TimeClass(int x2, int y2, std::string ourName)
    : lastTime(Timer::getCurrentTime()), callNum(0), x(x2), y(y2), name(ourName) {}

  virtual ~TimeClass() {}

  void timerCallback() {
    Time currTime = Timer::getCurrentTime();
    Time diffTime = currTime - lastTime;
    lastTime = currTime;
    Console::mlprintf(x, y, "(%i)Hello, I'm %s! change: %ius", callNum, name.c_str(), diffTime.getTotaluSec());
    callNum++;
  }

private:
  Time lastTime;
  int callNum;
  int x, y;
  std::string name;
};

int main() {
  GNE::init(NL_IP, atexit);
  Console::init(atexit);

  //Doing some tests on Time class
  Console::mprintf("Time class tests:\n");
  Time t(0, 1000000);
  Console::mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t += 1000001; //add a little over one second
  Console::mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t.setSec(5);
  Console::mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  t = t + Time(3, 5500000);
  Console::mprintf("%is, %ius\n", t.getSec(), t.getuSec());
  Console::mprintf("Timer class tests, press a key to quit:\n");

  //Create the timers
  Timer t1(new TimeClass(3, 8, "Bob"), 1000);
  Timer t2(new TimeClass(5, 10, "Sally"), 1250);
  Timer t3(new TimeClass(1, 12, "Joe"), 200);

  //Start the timers
  t1.startTimer();
  assert(t1.isRunning());
  t2.startTimer();
  assert(t2.isRunning());
  t3.startTimer();
  assert(t3.isRunning());

  while(!Console::kbhit()) { //Wait for keypress
    Thread::sleep(100); //sleep to give up the CPU for a bit.
  }

  Console::mlprintf(0, 14, "Shutting down timers, please wait...");
  t1.stopTimer();
  t2.stopTimer();
  t3.stopTimer();

  return 0;
}