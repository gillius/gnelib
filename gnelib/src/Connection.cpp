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
  "No error.",
  "The host has an earlier version of GNE than this one.",
  "The host has a later version of GNE than this one.",
  "The host has an earlier version of this game.",
  "The host has a later version of this game.",
  "Could not open a network connection, check to make sure you are connected to the network.",
  "Could not contact the host due to connection timeout.",
  "Other low-level HawkNL error."
};

//##ModelId=3AE3591A0186
Connection::Connection(int outRate, int inRate)
: ps(NULL), connected(false), rsocket(NL_INVALID), usocket(NL_INVALID) {
}

//##ModelId=3AE3591A0187
Connection::~Connection() {
  delete ps;
}

//##ModelId=3AE4A6F00280
PacketStream& Connection::stream() {
  return *ps;
}

//##ModelId=3AE4C9DD0280
Connection::Stats Connection::getStats() const {
  return Stats();
}

//##ModelId=3AFF798800C8
NLaddress Connection::getLocalAddress(bool reliable) const {
  NLaddress ret;
  if (reliable) {
    assert(rsocket != NL_INVALID);
    nlGetLocalAddr(rsocket, &ret);
  } else {
    assert(usocket != NL_INVALID);
    nlGetLocalAddr(usocket, &ret);
  }
  return ret;
}

//##ModelId=3AFF79880136
NLaddress Connection::getRemoteAddress(bool reliable) const {
  NLaddress ret;
  if (reliable) {
    assert(rsocket != NL_INVALID);
    nlGetRemoteAddr(rsocket, &ret);
  } else {
    assert(usocket != NL_INVALID);
    nlGetRemoteAddr(usocket, &ret);
  }
  return ret;
}

//##ModelId=3AE4A9820366
bool Connection::isConnected() const {
  return connected;
}

//##ModelId=3AE4A9700212
void Connection::disconnect() {
  if (rsocket != NL_INVALID)
    nlClose(rsocket);
  if (usocket != NL_INVALID)
    nlClose(usocket);
}

//##ModelId=3AE4C7FC021C
void Connection::disconnectSendAll() {
}

//##ModelId=3AE4CD3D00E6
void Connection::onFailure(FailureType errorType) {
}

//##ModelId=3AE4A776033E
void Connection::onReceive() {
}

//##ModelId=3AE4C8F501CC
void Connection::onDoneWriting() {
}

//##ModelId=3B009E0903AC
void Connection::onReceive(bool reliable) {
}

//##ModelId=3B009D63019A
Connection::ConnectionListener::ConnectionListener(Connection& listener, bool isReliable) 
: conn(listener), reliable(isReliable) {
}

//##ModelId=3B009D630208
Connection::ConnectionListener::~ConnectionListener() {
}

//##ModelId=3B0099B700BE
void Connection::ConnectionListener::onReceive() {
}

