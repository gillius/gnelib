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

std::ostream& operator << (std::ostream& o, const GNE::Error& err) {
	return o << err.toString();
}

/*
  enum ErrorCode {
    NoError,
    GNEHostVersionLow,
    GNEHostVersionHigh,
    UserHostVersionLow,
    UserHostVersionHigh,
    CouldNotOpenSocket,
    ConnectionTimeOut,
		ConnectionDropped,
		SyncConnectionReleased,
		Read,
		Write,
		UnknownPacket,
		PacketTypeMismatch,
		OtherGNELevelError,
    OtherLowLevelError
  };
*/

namespace GNE {

const std::string ErrorStrings[] = {
  "No error.",
  "The host has an earlier version of GNE than this one.",
  "The host has a later version of GNE than this one.",
  "The host has an earlier version of this game.",
  "The host has a later version of this game.",
  "Could not open a network connection, check to make sure you are connected to the network.",
  "Could not contact the host due to connection timeout.",
	"Remote computer suddenly disconnected without warning, or dropped (TCP) (in pre-alpha GNE, this is expected for now...)",
	"The operation was canceled because the SyncConnection is released.",
	"Network error when trying to read from connection.",
	"Network error when trying to write to connection.",
	"Unknown packet type encountered or corrupted data received -- possible additional data loss.",
	"Packet type received does not match next packet type expected.",
	"Other GNE (not a low-level network) error.",
  "Low-level HawkNL error:"
};

//##ModelId=3BAEC1A30050
Error::Error(ErrorCode ec) : code(ec), hawkError(NL_NO_ERROR), sysError(0) {
}

//##ModelId=3BAEC1A30051
Error::~Error() {
}

//##ModelId=3BAEC1A30053
Error::ErrorCode Error::getCode() const {
	return code;
}

//##ModelId=3BBA9D6E01E1
const Error& Error::setCode(Error::ErrorCode newCode) {
	code = newCode;
	return *this;
}

//##ModelId=3BAEC1DF014A
std::string Error::toString() const {
	std::stringstream ret;
	ret << ErrorStrings[code];
	if (hawkError != NL_NO_ERROR && hawkError != NL_SOCKET_ERROR)
		ret << " HawkNL error " << hawkError << ": " << nlGetErrorStr(hawkError);
	if (sysError != 0)
		ret << " System error " << sysError << ": " << nlGetSystemErrorStr(sysError);
	return ret.str();
}

//##ModelId=3BAEC39201C2
Error::operator bool() const {
	return (code == NoError);
}

//##ModelId=3BAEC74F0168
bool Error::operator == (const ErrorCode& rhs) const {
	return (code == rhs);
}

//##ModelId=3BBA9D6E02BC
Error Error::createLowLevelError() {
	Error ret(Error::NoError);
	NLenum error = nlGetError();
	if (error == NL_SOCKET_ERROR || error == NL_SOCK_DISCONNECT)
		ret.sysError = nlGetSystemError();
	else
		ret.hawkError = error;
	return ret;
}

}
