#ifndef CONNECTIONWRITER_H_INCLUDED_C51B679A
#define CONNECTIONWRITER_H_INCLUDED_C51B679A

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
#include "Thread.h"
class ConditionVariable;

/**
 * An internal GNE class to handle the low-level writing of data from a
 * PacketStream.  Users should not need to use or know this class.
 */
//##ModelId=3AE4BE8C0140
class ConnectionWriter : public Thread {
public:
  //##ModelId=3AE4C448001E
  ConnectionWriter(ConditionVariable& sync);

  //##ModelId=3AE4C448005A
  virtual ~ConnectionWriter();

  //##ModelId=3AE4C4B60140
  void run();

private:
  //##ModelId=3AE4CE840028
  int outLeft;

  //##ModelId=3AE4C3D50190
  ConditionVariable& writeSync;
};



#endif /* CONNECTIONWRITER_H_INCLUDED_C51B679A */


