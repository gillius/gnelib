#ifndef TIMERCALLBACK_H_INCLUDED_C517B12B
#define TIMERCALLBACK_H_INCLUDED_C517B12B

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
 * A pure interface class for usage in the Timer class.  Inherit from this
 * class and define timerCallback.
 */
//##ModelId=3AE868D6019A
class TimerCallback {
public:
  /**
   * The function that the Timer calls at its firing rate.
   */
  //##ModelId=3AE868E20334
  virtual void timerCallback() = 0;

};



#endif /* TIMERCALLBACK_H_INCLUDED_C517B12B */


