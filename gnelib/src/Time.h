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
  int getSec();

  /**
   * Returns microseconds.
   */
  //##ModelId=3AEBA4B100C8
  int getuSec();

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
  Time diff(const Time& rhs);

  /**
   * Less-than operator that works as expected.
   */
  //##ModelId=3AEBA4D200A0
  bool operator<(const Time& rhs);

  /**
   * Greater-than operator that works as expected.
   */
  //##ModelId=3AEBA4E003A2
  bool operator>(const Time& rhs);

  /**
   * Time addition that works as expected.
   */
  //##ModelId=3AEBA4F9021C
  Time operator+(const Time& rhs);

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


