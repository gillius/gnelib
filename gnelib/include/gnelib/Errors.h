#ifndef _ERRORS_H_
#define _ERRORS_H_

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

//This file contains various errors derived from Error.

#include "gneintern.h"
#include "Error.h"

namespace GNE {

/**
 * Adds low level HawkNL and possibly system error information to any other
 * error.
 */
//##ModelId=3C86E08F0344
class LowLevelError : public Error {
public:
  /**
   * Creates a normal error, but picks up any error information reported
   * currently by the underlying network library implementation.
   */
  //##ModelId=3C86E09000C4
  LowLevelError(ErrorCode newCode = OtherLowLevelError);

  //##ModelId=3C86E09000C6
  virtual std::string toString() const;

private:
  /**
   * A possible error code for HawkNL.
   */
  NLint hawkError;

  /**
   * A possible error code for the system given by HawkNL.
   */
  //##ModelId=3C86E09000C3
  int sysError;
};

/**
 * An error that represents a mismatch in the games during the connection
 * process.  If two GNE programs try to connect that are different games,
 * this is the error you will get.
 */
//##ModelId=3C86E090000E
class WrongGame : public Error {
public:
  //##ModelId=3C86E0900163
  WrongGame(std::string GameName);

  /**
   * Returns the mismatched game name.
   */
  //##ModelId=3C86E0900165
  std::string getWrongGame() const;

  //##ModelId=3C86E0900167
  virtual std::string toString() const;

private:
  std::string gameName;
};

/**
 * An error thrown by SyncConnection when you get a packet other than the one
 * you are trying to receive.
 */
//##ModelId=3C86E08F0369
class PacketTypeMismatch : public Error {
public:
  //##ModelId=3C86E09000E2
  PacketTypeMismatch(int OtherID)
    : Error(Error::PacketTypeMismatch), otherID(OtherID) {}

  /**
   * Returns the offending packet's ID.
   */
  //##ModelId=3C86E09000E4
  int getWrongID() const { return otherID; }

private:
  //##ModelId=3C86E09000E1
  int otherID;
};

/**
 * An error that occurs during the connecting process if the user versions
 * are different.  The version is passed to you because GNE does not know
 * what format your version is -- only that is is not equal.
 */
//##ModelId=3C86E0900004
class UserVersionMismatch : public Error {
public:
  //##ModelId=3C86E090015E
  UserVersionMismatch(guint32 OtherVer)
    : Error(Error::UserVersionMismatch), otherVer(OtherVer) {}

  /**
   * Returns the offending user version.
   */
  //##ModelId=3C86E0900160
  guint32 getWrongVer() const { return otherVer; }

private:
  //##ModelId=3C86E0AA0138
  guint32 otherVer;
};

/**
 * An error that occurs during the connection process if the remote machine
 * sends incorrect or corrupted packets to us.  This might be possible if
 * they were using a pre-alpha version of GNE, or if the connection came from
 * a non-GNE program.
 */
//##ModelId=3C86E08F037D
class ProtocolViolation : public Error {
public:
  //##ModelId=3C86E09000F4
  enum ViolationType {
    OtherViolation = 0,
    InvalidCRP,
    InvalidCAP,
    InvalidUnreliableInfo
  };

  //##ModelId=3C86E09000FB
  ProtocolViolation(ViolationType T = OtherViolation);

  //##ModelId=3C86E09000FD
  ViolationType getViolationType() const;
  //##ModelId=3C86E09000FF
  void setViolationType(ViolationType T);

  //##ModelId=3C86E0900101
  virtual std::string toString() const;

private:
  //##ModelId=3C86E0AA00D4
  ViolationType t;
};

}

#endif