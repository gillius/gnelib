#ifndef _CONSOLESTREAM_H_
#define _CONSOLESTREAM_H_

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
namespace GNE {
class Mutex;

namespace Console {

//##ModelId=3BF8BBF90219
class ConsoleManipulator {
public:
  //##ModelId=3BF8BBF90278
  virtual ~ConsoleManipulator() {};

  //##ModelId=3BF8BBF9027A
  virtual void action(std::ostream& o) = 0;
};

/**
 * A class for syncronizing the gout stream.  You won't create this class
 * directly, but instead will use the acquire and release variables.
 */
//##ModelId=3BF88CAB021F
class ConsoleMutex : public ConsoleManipulator {
public:
  //##ModelId=3BF88CAB02E7
  ConsoleMutex(bool isAcquiring, Mutex& syncMutex);
  //##ModelId=3BF88CAB02EA
  virtual ~ConsoleMutex();

  //Perform a release or an acquire, based on acq.
  //##ModelId=3BF88CAB02EB
  void action(std::ostream& o);

private:
  //acq is true if we are trying to acquire, false if release.
  //##ModelId=3BF88CAB02D8
  bool acq;

  //sync is the mutex we are wrapping
  //##ModelId=3BF88CC1010E
  Mutex& sync;
};

/**
 * A manipulator for gout essentially meant to facilitate a C++ iostream
 * version of mlprintf.  Using moveTo in the gout stream (and ONLY gout),
 * will set the stream to display the next line-buffered text using an
 * mlprintf rather than a normal mprintf.  gout needs to be acquired if other
 * threads might write to gout.  The following code:\n
 * gout << acquire << moveTo(10, 15) << "Hello, World!" << flush << release;
 * \nworks like mlprintf(10, 15, "Hello, World!");.  Other stuff may be
 * before if anything is in the stream -- remember gout, like cout, only
 * displays when it encounters a newline or is explicitly flushed.
 */
//##ModelId=3BF8BBF9021D
class moveTo : public ConsoleManipulator {
public:
  //##ModelId=3BF8BBF9034D
  moveTo(int xLoc, int yLoc);
  //##ModelId=3BF8BBF90350
  virtual ~moveTo();

  //##ModelId=3BF8BBF90352
  void action(std::ostream& o);
private:
  //##ModelId=3BF8BBF9034B
  int x, y;
};

}
}

std::ostream& operator << (std::ostream& o, GNE::Console::ConsoleManipulator& cm);

#endif
