#ifndef GNE_H_INCLUDED_C51DF1DD
#define GNE_H_INCLUDED_C51DF1DD

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
#include "ConnectionStats.h"

/**
 * The namespace in which all of GNE resides in.  The namespace consists of
 * a few global functions, some constants, classes, and the GNE::Console
 * namespace, which holds the console IO functions.
 */
//##ModelId=3AE26ED703DE
namespace GNE {
class Address;
class ConnectionEventGenerator;
  /**
   * Initalizes GNE and HawkNL.  Call this before using any HawkNL or GNE
   * functions.  Pass it the atexit function so shutdown will be called on
   * exit.  A call to any other GNE function before this function succeeds
   * is undefined.
   * @param networkType a HawkNL network driver, such as NL_IP or NL_IPX
   * @return true if GNE or HawkNL could not be initalized.
   */
  bool initGNE(NLenum networkType, int (*atexit_ptr)(void (*func)(void)));

  /**
   * Shutsdown GNE and HawkNL.  This function should be called only after all
   * connections are individually shutdown to properly deinitalize the
   * library.  However, this function is acceptable enough to be used on an
   * "emergency" shutdown due to some error -- just don't try to call any
   * other GNE or HawkNL function after calling this.
   * Note that it is possible that this function may block for a short time
   * (500ms or less), while the event generators shut down.
   */
  void shutdownGNE();

  /**
   * Use this function to get the address of the default networking device on
   * this system, if possible.  The port in the resulting address will be
   * zero.  The returned address is invalid if an error occured.
   */
  Address getLocalAddress();

  /**
   * Exits from your main() function.  This should be used if you want any
   * detached threads to continue running.  All threads must either be
   * detached or joined.
   * @see Thread::detach()
   * @see Thread::join()
   * @param status the return code
   */
  //static void exit(int status);


  /**
   * Gets the global stats.  Very similar to Connection::getStats, but this
   * gets cumulative stats for all sockets, user-created ones and GNE ones
   * too you may not know about.
   */
  ConnectionStats getGlobalStats();

  /**
   * Enables stats gathering, which is off by default.  When stats is not
   * enabled, the get function's results is undefined.
   */
  void enableStats();

  /**
   * Disables stats gathering, which is the default mode.
   */
  void disableStats();

  /**
   * Clears all global stats.
   */
  void clearStats();
  
  /**
   * Returns the number of open low-level connections GNE has opened.
   */
  int getOpenConnections();

  /**
   * Sets the user application protocol version number.
   * @see userVersion
   */
  void setUserVersion(int version);

  /**
   * A numeric representation of the current version of this library.
   */
  const double VER = 0.45;

  /**
   * A string representation of the name of the library and the current
   * version.
   */
  const std::string VER_STR = "GNE v0.46 alpha CVS";

  /**
   * The low-level header size of a UDP header, which is used with the HawkNL
   * internet driver that GNE uses.  A number that I always have trouble
   * finding and might be useful in getting stats or calculating actual
   * bandwith.
   */
  const int UDP_HEADER_SIZE = 28;

  /**
   * Normally you would pass a network type to GNE::init, but passing this
   * value means that no network should be initalized.  This is useful only
   * in the example programs that don't use networking and do not need to
   * initalize everything.
   * @see initGNE
   */
  const NLenum NO_NET = 128;

  /**
   * The user's own protocol version number, 0 by default.  If you set this
   * number, GNE will version check not only its own library version, but
   * your application version as well on connect, and produce the appropriate
   * error if there is a mismatch.\n
   * To set this number, use setUserVersion(int).
   */
  extern int userVersion;

  /**
   * The global event generator.  The library uses this internally to
   * register and unregister connections.  The end-user will not have to use
   * this object under any normal cirumstances.
   */
  extern ConnectionEventGenerator* eGen;
}

#endif /* GNE_H_INCLUDED_C51DF1DD */



