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

#include "ConnectionStats.h"
#include "Error.h"
#include "gnetypes.h"

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
   *
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

//##ModelId=3C5CED0500EC
  struct GNEProtocolVersionNumber {
  //##ModelId=3C65C7A20213
    guint8 version;
  //##ModelId=3C65C7A20218
    guint8 subVersion;
  //##ModelId=3C65C7A20220
    guint16 build;
  };

  /**
   * Returns the GNE Protocol version number.  This is different from the
   * library version number.  This number is specified by the GNE protocol
   * document on the GNE web site.  Versions of this library obtained from
   * CVS might have the build number non-zero.  Non-beta public releases of
   * GNE will have a build number of zero and a version and subVersion number
   * strictly defined by the GNE protocol specification.  Public alpha or
   * beta releases might have a non-zero build number.
   */
  GNEProtocolVersionNumber getGNEProtocolVersion();

  /**
   * This function is used internally by GNE to get the name that you set in
   * setGameInformation.
   */
  const char* getGameName();

  /**
   * This function is used internally by GNE to get the version that you set
   * in setGameInformation.
   */
  guint32 getUserVersion();

  /**
   * The user's game information.  This information is used during the
   * connection process to check if the versions and games match.  This
   * function should only be called once before making any connections.
   *
   * @param gameName the name of your game, or any other unique string
   *        identifier in ASCII format with a length not exceeding
   *        MAX_GAME_NAME_LEN ASCII characters.
   * @param version a 4-byte value with the current game version in any
   *        format.
   */
  void setGameInformation(std::string gameName, guint32 version);

  /**
   * The maximum number of ASCII characters in game name that is passed into
   * the setGameInformation function.
   */
  const int MAX_GAME_NAME_LEN = 31;

  /**
   * Compares other versions against this library.  This is used internally
   * by GNE to compare versions.  It checks the GNE version numbers first,
   * then the game name, then the user versions.
   *
   * @throw Error if the GNE versions did not match.
   * @throw WrongGame if the game is of the wrong type.
   * @throw UserVersionMismatch if the user versions are different.
   */
  void checkVersions(const GNEProtocolVersionNumber& otherGNE,
    std::string otherName, guint32 otherUser);

  /**
   * A numeric representation of the current version of this library.
   */
  const double VER = 0.56;

  /**
   * A string representation of the name of the library and the current
   * version.
   */
  const std::string VER_STR = "GNE v0.56 alpha CVS";

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
   * The global event generator.  The library uses this internally to
   * register and unregister connections.  The end-user will not have to use
   * this object under any normal cirumstances.
   */
  extern ConnectionEventGenerator* eGen;
}

#endif /* GNE_H_INCLUDED_C51DF1DD */
