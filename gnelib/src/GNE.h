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
   * functions.
   * @param networkType a HawkNL network driver, such as NL_IP or NL_IPX
   * @return true if GNE or HawkNL could not be initalized.
   */
  //##ModelId=3AE270BF0078
  static bool init(NLenum networkType);

  /**
   * Shutsdown GNE and HawkNL.  This function should be called only after all
   * connections are individually shutdown to properly deinitalize the
   * library.  However, this function is acceptable enough to be used on an
   * "emergency" shutdown due to some error -- just don't try to call any
   * other GNE or HawkNL function after calling this.
   */
  //##ModelId=3AE270CE033E
  static void shutdown();

  /**
   * Exits from your main() function.  This should be used if you want any
   * detached threads to continue running.  All threads must either be
   * detached or joined.
   * @see Thread::detach()
   * @see Thread::join()
   * @param status the return code
   */
  //##ModelId=3AEDEDE100AA
  static void exit(int status);

  /**
   * Returns how many total bytes GNE has read on all sockets.
   */
  //##ModelId=3AE4C97C0244
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
   * Returns the number of open low-level connections GNE has opened.
   */
  //##ModelId=3AE4D10803CA
  static int getOpenConnections();

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

  //##ModelId=3AE4DF7A03AC
  static const int UDP_HEADER_SIZE;

private:
  //##ModelId=3AE34D930316
  ConnectionEventGenerator *eGen;
};

#endif /* GNE_H_INCLUDED_C51DF1DD */


