#ifndef GNE_H_INCLUDED_C51DF1DD
#define GNE_H_INCLUDED_C51DF1DD

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
class ConnectionEventGenerator;

/**
 * The main class for the library, containing all static members and methods
 * that do not apply directly to any other class.
 */
//##ModelId=3AE26ED703DE
class GNE {
public:
  /**
   * Initalizes GNE and HawkNL.  Call this before using any HawkNL or GNE
   * functions.  Pass it the atexit function so shutdown will be called on
   * exit.  This is so the main thread can exit before the threads it
   * created.
   * @param networkType a HawkNL network driver, such as NL_IP or NL_IPX
   * @return true if GNE or HawkNL could not be initalized.
   */
  //##ModelId=3AE270BF0078
  static bool init(NLenum networkType, int (*atexit_ptr)(void (*func)(void)));

  /**
   * Shutsdown GNE and HawkNL.  This function should be called only after all
   * connections are individually shutdown to properly deinitalize the
   * library.  However, this function is acceptable enough to be used on an
   * "emergency" shutdown due to some error -- just don't try to call any
   * other GNE or HawkNL function after calling this.
   * Note that it is possible that this function may block for a short time
   * (500ms or less), while the event generators shut down.
   */
  //##ModelId=3AE270CE033E
  static void shutdown();

  /**
   * Use this function to get the address of the default networking device on
   * this system, if possible.
   */
  //##ModelId=3AFF79890208
  static NLaddress getLocalAddress();

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
   * Returns how many total bytes GNE has read on all sockets.
   */
  //##ModelId=3AEDEDE100AA
  static int getBytesRead();

  /**
   * Returns how many total bytes GNE has written on all sockets.
   */
  //##ModelId=3AE4C988000A
  static int getBytesWritten();

  /**
   * Enables stats gathering, which is off by default.  When stats is not
   * enabled, the get function's results is undefined.
   */
  //##ModelId=3AE4CA1D0122
  static void enableStats();

  /**
   * Disables stats gathering, which is the default mode.
   */
  //##ModelId=3AE4CA2803AC
  static void disableStats();

  /**
   * Clears all global stats.
   */
  //##ModelId=3AFF64290316
  static void clearStats();
  
  /**
   * Returns the number of open low-level connections GNE has opened.
   */
  //##ModelId=3AE4D10803CA
  static int getOpenConnections();

  /**
   * Sets the user application protocol version number.
   * @see userVersion
   */
  //##ModelId=3AFF642A000A
  static void setUserVersion(int version);

  /**
   * A numeric representation of the current version of this library.
   */
  //##ModelId=3AE26F6A035C
  static const double VER;

  /**
   * A string representation of the name of the library and the current
   * version.
   */
  //##ModelId=3AE26FC000A0
  static const std::string VER_STR;

  /**
   * The low-level header size of a UDP header, which is used with the HawkNL
   * internet driver that GNE uses.  A number that I always have trouble
   * finding and might be useful in getting stats or calculating actual
   * bandwith.
   */
  //##ModelId=3AE4DF7A03AC
  static const int UDP_HEADER_SIZE;

  /**
   * Normally you would pass a network type to GNE::init, but passing this
   * value means that no network should be initalized.  This is useful only
   * in the example programs that don't use networking and do not need to
   * initalize everything.
   * @see init
   */
  //##ModelId=3AFF3E3500FA
  static const NLenum NO_NET;

  /**
   * The user's own protocol version number, 0 by default.  If you set this
   * number, GNE will version check not only its own library version, but
   * your application version as well on connect, and produce the appropriate
   * error if there is a mismatch.
   */
  //##ModelId=3AFF6429023A
  static int userVersion;

private:
  //##ModelId=3AF8D89F00E6
  static bool initialized;

  //##ModelId=3AE34D930316
  ConnectionEventGenerator *eGen;

};

#endif /* GNE_H_INCLUDED_C51DF1DD */


