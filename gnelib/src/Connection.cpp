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

//##ModelId=3AE5BA8F038E
static const std::string FailureStrings[] = {
  "The host has an earlier version of GNE than this one.",
  "The host has a later version of GNE than this one.",
  "The host has an earlier version of this game.",
  "The host has a later version of this game.",
  "Could not contact the host.",
  "Could not open a network connection, check to make sure you are connected to the network.",
  "Other low-level HawkNL error."
};

//##ModelId=3AE3591A0186
Connection::Connection(int outRate, int inRate)
: ps(outRate, inRate), connected(false), socket(NL_INVALID) {
}

//##ModelId=3AE3591A0187
Connection::~Connection() {
}

//##ModelId=3AE4A6F00280
PacketStream& Connection::stream() {
  return ps;
}

//##ModelId=3AE4C9DD0280
Connection::Stats Connection::getStats() const {
  assert(socket != NL_INVALID);
  return Stats();
}

//##ModelId=3AFF798800C8
NLaddress Connection::getLocalAddress() const {
  assert(socket != NL_INVALID);
  NLaddress ret;
  nlGetLocalAddr(socket, &ret);
  return ret;
}

//##ModelId=3AFF79880136
NLaddress Connection::getRemoteAddress() const {
  assert(socket != NL_INVALID);
  NLaddress ret;
  nlGetRemoteAddr(socket, &ret);
  return ret;
}

//##ModelId=3AE4A9820366
bool Connection::isConnected() const {
  return connected;
}

//##ModelId=3AE4A9700212
void Connection::disconnect() {
  assert(socket != NL_INVALID);
  nlClose(socket);
}

//##ModelId=3AE4C7FC021C
void Connection::disconnectSendAll() {
}

//##ModelId=3AE4CD3D00E6
void Connection::onFailure(FailureType errorType) {
}

//##ModelId=3AE4A776033E
void Connection::onRecieve() {
}

//##ModelId=3AE4C8F501CC
void Connection::onDoneWriting() {
}




