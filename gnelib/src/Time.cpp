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

#include "gneintern.h"
#include "Time.h"

//##ModelId=3AEBA04C03A2
Time::Time() : sec(0), microsec(0) {
}

//##ModelId=3AEEF7680320
Time::Time(int seconds, int microseconds) 
: sec(seconds), microsec(microseconds) {
  normalize();
}

//##ModelId=3AEBA04C03DE
Time::~Time() {
}

//##ModelId=3AEBA4AC0096
int Time::getSec() {
  return sec;
}

//##ModelId=3AEBA4B100C8
int Time::getuSec() {
  return microsec;
}

//##ModelId=3AF9D5750082
int Time::getTotaluSec() {
  return (sec * 1000000 + microsec);
}

//##ModelId=3AEBA4BE0226
void Time::setSec(int seconds) {
  sec = seconds;
}

//##ModelId=3AEBA4C80032
void Time::setuSec(int microseconds) {
  microsec = microseconds;
  normalize();
}

//##ModelId=3AEBA53B00AA
Time Time::diff(const Time& rhs) {
  Time ret = operator-(rhs);
  ret.sec = labs(ret.sec);
  return ret;
}

//##ModelId=3AEBA4D200A0
bool Time::operator<(const Time& rhs) {
  return (sec < rhs.sec || ((sec == rhs.sec) && (microsec < rhs.microsec)));
}

//##ModelId=3AEBA4E003A2
bool Time::operator>(const Time& rhs) {
  return (sec > rhs.sec || ((sec == rhs.sec) && (microsec > rhs.microsec)));
}

//##ModelId=3AEBA4F9021C
Time Time::operator+(int rhs) {
  Time ret(*this);
  ret.microsec += rhs;
  ret.normalize();
  return ret;
}

//##ModelId=3AF9D5750122
void Time::operator+=(int rhs) {
  microsec += rhs;
  normalize();
}

//##ModelId=3AF9D57503B6
Time Time::operator+(const Time& rhs) {
  Time t(sec + rhs.sec, microsec + rhs.microsec);
  t.normalize();
  return t;
}

Time Time::operator -(const Time& rhs) {
  Time t(sec - rhs.sec, microsec - rhs.microsec);
  t.normalize();
  return t;
}

//##ModelId=3AECF2860320
void Time::normalize() {
  if (microsec > 999999) {
    sec += (microsec / 1000000);
    microsec = microsec % 1000000;
  } else while (microsec < 0) {
    sec--;
    microsec += 1000000;
  }
}

