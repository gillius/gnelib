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
#include "../include/gnelib/Time.h"

namespace GNE {

//##ModelId=3B07538103AF
Time::Time() : sec(0), microsec(0) {
}

//##ModelId=3B07538103B0
Time::Time(int seconds, int microseconds) 
: sec(seconds), microsec(microseconds) {
  normalize();
}

//##ModelId=3B07538103B3
Time::~Time() {
}

//##ModelId=3B07538103B5
int Time::getSec() const {
  return sec;
}

//##ModelId=3B07538103B7
int Time::getuSec() const {
  return microsec;
}

//##ModelId=3B07538103DE
int Time::getTotaluSec() const {
  return (sec * 1000000 + microsec);
}

//##ModelId=3B07538103E0
void Time::setSec(int seconds) {
  sec = seconds;
}

//##ModelId=3B07538103E2
void Time::setuSec(int microseconds) {
  microsec = microseconds;
  normalize();
}

//##ModelId=3B07538103E4
Time Time::diff(const Time& rhs) const {
  Time ret = operator-(rhs);
  ret.sec = labs(ret.sec);
  return ret;
}

//##ModelId=3B07538103E7
bool Time::operator<(const Time& rhs) const {
  return (sec < rhs.sec || ((sec == rhs.sec) && (microsec < rhs.microsec)));
}

//##ModelId=3AEBA4E003A2
bool Time::operator>(const Time& rhs) const {
  return (sec > rhs.sec || ((sec == rhs.sec) && (microsec > rhs.microsec)));
}

//##ModelId=3B07538103ED
Time Time::operator+(int rhs) const {
  Time ret(*this);
  ret.microsec += rhs;
  ret.normalize();
  return ret;
}

//##ModelId=3B07538103F0
void Time::operator+=(int rhs) {
  microsec += rhs;
  normalize();
}

//##ModelId=3B07538103F2
Time Time::operator+(const Time& rhs) const {
  Time t(sec + rhs.sec, microsec + rhs.microsec);
  t.normalize();
  return t;
}

Time Time::operator -(const Time& rhs) const {
  Time t(sec - rhs.sec, microsec - rhs.microsec);
  t.normalize();
  return t;
}

//##ModelId=3B07538103F8
void Time::normalize() {
  if (microsec > 999999) {
    sec += (microsec / 1000000);
    microsec = microsec % 1000000;
  } else while (microsec < 0) {
    sec--;
    microsec += 1000000;
  }
}

}






