#ifndef TIME_H_INCLUDED_C51412BE
#define TIME_H_INCLUDED_C51412BE

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

//This include file has to be named strangly because of a conflict with the
//standard C include time.h.  Both MSVC and GCC have given us problems with
//including this file, despite the usage between #include <> and #include ""

#include "gneintern.h"
class ostream;

namespace GNE {

/**
 * A class representing a time relative to a point in the past.  The time
 * is always proper ("normalized"), meaning that 999,999 is the highest value
 * for microseconds.  Setting a value higher than this will increase seconds.
 */
//##ModelId=3B075380037A
class Time {
public:
  /**
   * Initalizes this class with a time of 0.
   */
  //##ModelId=3B07538103AF
  Time();

  /**
   * Initalizes this class with the time specified.  The given time need not
   * be normalized as this will be done automatically.  For example it is
   * acceptable to pass in 0 seconds and 2,000,000 microseconds and it will
   * be normalized to 2 seconds.
   */
  //##ModelId=3B07538103B0
  Time(int seconds, int microseconds);

  //##ModelId=3B07538103B3
  virtual ~Time();

  /**
   * Returns seconds.
   */
  //##ModelId=3B07538103B5
  int getSec() const;

  /**
   * Returns microseconds.
   */
  //##ModelId=3B07538103B7
  int getuSec() const;

  /**
   * Returns the total time in microseconds.  This function can be useful
   * for displaying the time represented by this object in a custom format
   * (such as xxxxx ms or xxxxx us, rather than xxx.xxx seconds).  Since an
   * int is returned, 2147.483648 seconds is the maximum amount of time that
   * can be represented in a 32-bit integer in microseconds.
   */
  //##ModelId=3B07538103DE
  int getTotaluSec() const;

  /**
   * Sets seconds.
   */
  //##ModelId=3B07538103E0
  void setSec(int seconds);

  /**
   * Sets microseconds.
   * @param microseconds the new value for microseconds.  If this value is
   *                     greater that 999,999, it will be normalized and the
   *                     value of seconds will increase.
   */
  //##ModelId=3B07538103E2
  void setuSec(int microseconds);

  /**
   * Returns the absolute value of the difference between these two times.
   */
  //##ModelId=3B07538103E4
  Time diff(const Time& rhs) const;

  /**
   * Returns a string representation of this time in seconds, for example
   * this might return 5.002052 if there was 5 seconds and 2052 microseconds.
   */
  std::string toString() const;

  /**
   * Less-than operator that works as expected.
   */
  //##ModelId=3B07538103E7
  bool operator<(const Time& rhs) const;

  /**
   * Greater-than operator that works as expected.
   */
  //##ModelId=3B07538103EA
  bool operator>(const Time& rhs) const;

  /**
   * Time addition, adding microseconds of time.
   */
  //##ModelId=3B07538103ED
  Time operator+(int rhs) const;

  /**
   * Time addition, adding microseconds of time to this object.
   */
  //##ModelId=3B07538103F0
  void operator+=(int rhs);

  /**
   * Time addition that works as expected.
   */
  //##ModelId=3B07538103F2
  Time operator+(const Time& rhs) const;

  /**
   * Time subtraction that works as expected.  You might get negative values
   * out of this.  Sometimes Time::diff will be what you want, instead.
   * @see diff
   */
  //##ModelId=3B07538103F5
  Time operator-(const Time& rhs) const;

protected:
  /**
   * make sure that microsec stays under a second, adding to sec if needed.
   */
  //##ModelId=3B07538103F8
  void normalize();

private:
  //##ModelId=3B07538103AD
  int sec;

  //##ModelId=3B07538103AE
  int microsec;
};

}

/**
 * Sends the result of toString to the ostream.
 */
std::ostream& operator << (std::ostream& o, const GNE::Time& time);

#endif /* TIME_H_INCLUDED_C51412BE */
