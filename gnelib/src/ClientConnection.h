#ifndef CLIENTCONNECTION_H_INCLUDED_C51A7F46
#define CLIENTCONNECTION_H_INCLUDED_C51A7F46

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
#include "Connection.h"
#include "Thread.h"

/**
 * A class that can connect to a remote target.  Inherit from this class,
 * overriding the functions from Connection and ClientConnection based on the
 * events you wish to respond to.
 */
//##ModelId=3AE59EBB001E
class ClientConnection : public Connection, public Thread {
public:
  /**
   * @see Connection#Connection(int, int, std::string)
   */
  //##ModelId=3AE59FAB0000
  ClientConnection(int outRate, int inRate, std::string address);

  //##ModelId=3AE59FAB003C
  virtual ~ClientConnection();

  /**
   * Don't call this method.  Connection starts a new thread lasting only
   * while it is connecting.
   * @see Thread#run()
   */
  //##ModelId=3AE5AF7A0384
  void run();

  /**
   * Starts connection to the specified target.  This method does not block,
   * and a thread will be started to handle the connection process.
   * onConnect() or onConnectFailure() will be called depending on the
   * outcome of this process.
   * The version number for the library and your own version number will be
   * checked during this phase.  If either protocol versions mismatch,
   * onConnectFailure() will be triggered.
   * @see #onConnect()
   * @see #onConnectFailure()
   */
  //##ModelId=3AE59FD4019A
  void connect();

  /**
   * Event triggered after there is a successful connection.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3AE59FBB01A4
  virtual void onConnect();

  /**
   * Event triggered when a connection failed.
   * Note you must call this function explicity from your overridden
   * function FIRST so the underlying functions recieve this event.
   */
  //##ModelId=3AE59FCA0168
  virtual void onConnectFailure(FailureType errorType);

};



#endif /* CLIENTCONNECTION_H_INCLUDED_C51A7F46 */


