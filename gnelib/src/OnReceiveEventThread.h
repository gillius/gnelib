#ifndef ONRECEIVEEVENTTHREAD_H_INCLUDED_C491E34C
#define ONRECEIVEEVENTTHREAD_H_INCLUDED_C491E34C

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
#include "Thread.h"

namespace GNE {
class ConnectionEventListener;

/**
 * A class used internally by ConnectionEventGenerator to start each
 * onReceive event in a new thread, so that it can continue processing data
 * for other connections.
 */
//##ModelId=3B6E14D10258
class OnReceiveEventThread : public Thread {
public:
  //##ModelId=3B6E15A2024E
  OnReceiveEventThread(ConnectionEventListener& listener2);

  //##ModelId=3B6E15A2024F
  virtual ~OnReceiveEventThread();

  //##ModelId=3B6E15A701AE
  void run();

private:
  //##ModelId=3B6E161703CA
  ConnectionEventListener& listener;
};

} // namespace GNE

#endif /* ONRECEIVEEVENTTHREAD_H_INCLUDED_C491E34C */
