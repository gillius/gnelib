#ifndef TIME_H_INCLUDED_C51412BE
#define TIME_H_INCLUDED_C51412BE

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

/**
 * A class representing a time relative to a point in the past.  The time
 * is always proper ("normalized"), meaning that 999,999 is the highest value
 * for microseconds.  Setting a value higher than this will increase seconds.
 */
//##ModelId=3AEB9AD500A0
class Time {
public:
  /**
   * Initalizes this class with a time of 0.
   */
  //##ModelId=3AEBA04C03A2
  Time();

  /**
   * Initalizes this class with the time specified.
   */
  //##ModelId=3AEEF7680320
  Time(int seconds, int microseconds);

  //##ModelId=3AEBA04C03DE
  virtual ~Time();

  /**
   * Returns seconds.
   */
  //##ModelId=3AEBA4AC0096
  int getSec() const;

  /**
   * Returns microseconds.
   */
  //##ModelId=3AEBA4B100C8
  int getuSec() const;

  /**
   * Returns the total time in microseconds.  This function can be useful
   * for displaying the time represented by this object in a custom format
   * (such as xxxxx ms or xxxxx us, rather than xxx.xxx seconds).  Since an
   * int is returned, 2147.483648 seconds is the maximum amount of time that
   * can be represented in a 32-bit integer in microseconds.
   */
  //##ModelId=3AF9D5750082
  int getTotaluSec() const;

  /**
   * Sets seconds.
   */
  //##ModelId=3AEBA4BE0226
  void setSec(int seconds);

  /**
   * Sets microseconds.
   * @param microseconds the new value for microseconds.  If this value is
   *                     greater that 999,999, it will be normalized and the
   *                     value of seconds will increase.
   */
  //##ModelId=3AEBA4C80032
  void setuSec(int microseconds);

  /**
   * Returns the absolute value of the difference between these two times.
   */
  //##ModelId=3AEBA53B00AA
  Time diff(const Time& rhs) const;

  /**
   * Less-than operator that works as expected.
   */
  //##ModelId=3AEBA4D200A0
  bool operator<(const Time& rhs) const;

  /**
   * Greater-than operator that works as expected.
   */
  //##ModelId=3AEBA4E003A2
  bool operator>(const Time& rhs) const;

  /**
   * Time addition, adding microseconds of time.
   */
  //##ModelId=3AEBA4F9021C
  Time operator+(int rhs) const;

  /**
   * Time addition, adding microseconds of time to this object.
   */
  //##ModelId=3AF9D5750122
  void operator+=(int rhs);

  /**
   * Time addition that works as expected.
   */
  //##ModelId=3AF9D57503B6
  Time operator+(const Time& rhs) const;

  /**
   * Time subtraction that works as expected.  You might get negative values
   * out of this.  Sometimes Time::diff will be what you want, instead.
   * @see diff
   */
  //##ModelId=3AF9D57601F4
  Time operator-(const Time& rhs) const;

protected:
  /**
   * make sure that microsec stays under a second, adding to sec if needed.
   */
  //##ModelId=3AECF2860320
  void normalize();

private:
  //##ModelId=3AEBA0200262
  int sec;

  //##ModelId=3AEBA03501E0
  int microsec;
};



#endif /* TIME_H_INCLUDED_C51412BE */


