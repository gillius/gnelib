#ifndef ERROR_H_INCLUDED_C4514CDC
#define ERROR_H_INCLUDED_C4514CDC

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

/**
 * This class resembles some sort of error in GNE, with the ability to obtain
 * a textual description of the error.
 */
//##ModelId=3BAEC0E901EA
class Error {
public:
  /**
   * An enum describing connection failure type.
   */
  //##ModelId=3BAEC99B01FE
  enum ErrorCode {
    NoError = 0,
    GNEHostVersionLow = 1,
    GNEHostVersionHigh = 2,
    UserHostVersionLow = 3,
    UserHostVersionHigh = 4,
    CouldNotOpenSocket = 5,
    ConnectionTimeOut = 6,
		ConnectionDropped = 7,
		Read = 8,
		Write = 9,
		UnknownPacket = 10,
		OtherGNELevelError = 11,
    OtherLowLevelError = 12
  };

	/**
	 * Create a new object representing a certain error, given by the passed
	 * ErrorCode.  If no error code is passed, NoError is assumed.
	 */
  //##ModelId=3BAEC1A30050
  Error(ErrorCode ec = NoError);

  //##ModelId=3BAEC1A30051
  ~Error();

	/**
	 * Returns the error code for this error.
	 */
  //##ModelId=3BAEC1A30053
  ErrorCode getCode() const;

	/**
	 * Returns a string description of this error.  When possible, the string
	 * is "programmer-friendly," so you will probably want to only use this to
	 * write a debug message or out to a log, and tell the user more about the
	 * error.
	 */
  //##ModelId=3BAEC1DF014A
  std::string getDesc() const;

	/**
	 * Returns error.getCode() != NoError, a handy shortcut so you can test for
	 * error by if (error).
	 */
  //##ModelId=3BAEC39201C2
  operator bool() const;

	/**
	 * A handy shortcut for (error.getCode() == rhs).
	 * @param rhs an errorCode to test with.
	 */
  //##ModelId=3BAEC74F0168
  bool operator == (ErrorCode rhs) const;

private:
  //##ModelId=3BAEC9C101CD
  ErrorCode code;
};

} // namespace GNE

#endif /* ERROR_H_INCLUDED_C4514CDC */