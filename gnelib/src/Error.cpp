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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/Error.h"

#ifdef OLD_CPP
#include <iostream>
#else
#include <ostream>
#endif

std::ostream& operator << (std::ostream& o, const GNE::Error& err) {
  return o << err.toString();
}

/*
  enum ErrorCode {
    NoError,
    ProtocolViolation,
    GNETheirVersionLow,
    GNETheirVersionHigh,
    WrongGame,
    UserVersionMismatch,
    CouldNotOpenSocket,
    ConnectionTimeOut,
    ConnectionRefused,
    ConnectionDropped,
    SyncConnectionReleased,
    ExitNoticeReceived,
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
  "The remote end did not follow the specifications of the GNE protocol.",
  "The remote end uses an earlier version of the GNE protocol.",
  "The remote end uses a later version of the GNE protocol.",
  "The connecting game names differ.",
  "The user protocol version numbers do not match.",
  "Could not open a network socket.",
  "Could not contact the host due to connection timeout.",
  "The remote end decided to refuse your connection request.",
  "Remote computer suddenly disconnected without warning, or dropped.",
  "The operation was canceled because the SyncConnection is released.",
  "The remote end sent a connection close notice.",
  "Network error when trying to read from connection.",
  "Network error when trying to write to connection.",
  "Unknown packet type encountered or corrupted data received -- possible additional data loss.",
  "Packet type received does not match next packet type expected.",
  "Other GNE (not a low-level network) error.",
  "Low-level HawkNL error:"
};

Error::Error(ErrorCode ec) : code(ec) {
}

Error::~Error() {
}

Error::ErrorCode Error::getCode() const {
  return code;
}

const void Error::setCode(Error::ErrorCode newCode) {
  code = newCode;
}

std::string Error::toString() const {
  return ErrorStrings[code];
}

Error::operator bool() const {
  return (code == NoError);
}

bool Error::operator == (const ErrorCode& rhs) const {
  return (code == rhs);
}

} //namespace GNE
