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
#include "ErrorGne.h"

/*
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
  };*/

const std::string ErrorStrings[] = {
  "No error.",
  "The host has an earlier version of GNE than this one.",
  "The host has a later version of GNE than this one.",
  "The host has an earlier version of this game.",
  "The host has a later version of this game.",
  "Could not open a network connection, check to make sure you are connected to the network.",
  "Could not contact the host due to connection timeout.",
	"Remote computer suddenly disconnected without warning, or dropped (TCP) (in pre-alpha GNE, this is expected for now...)",
	"Network error when trying to read from connection.",
	"Network error when trying to write to connection.",
	"Unknown packet type encountered or corrupted data received.",
	"Other GNE (not a low-level network) error.",
  "Other low-level HawkNL error."
};

//##ModelId=3BAEC1A30050
GNE::Error::Error(ErrorCode ec) : code(ec) {
}

//##ModelId=3BAEC1A30051
GNE::Error::~Error() {
}

//##ModelId=3BAEC1A30053
GNE::Error::ErrorCode GNE::Error::getCode() const {
	return code;
}

//##ModelId=3BAEC1DF014A
std::string GNE::Error::getDesc() const {
	return ErrorStrings[code];
}

//##ModelId=3BAEC39201C2
GNE::Error::operator bool() const {
	return (code == NoError);
}

//##ModelId=3BAEC74F0168
bool GNE::Error::operator == (ErrorCode rhs) const {
	return (code == rhs);
}