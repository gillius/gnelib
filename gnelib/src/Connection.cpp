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
#include "Connection.h"
#include "RawPacket.h"
#include "PacketParser.h"
#include "ConnectionEventGenerator.h"
#include "GNE.h"

namespace GNE {

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

//##ModelId=3B0753810073
Connection::Connection(int outRate, int inRate)
: connected(false), rsocket(NL_INVALID), usocket(NL_INVALID),
rlistener(NULL), ulistener(NULL) {
	ps = new PacketStream(outRate, inRate, *this);
}

//##ModelId=3B0753810076
Connection::~Connection() {
	unreg(true, true);
  if (rsocket != NL_INVALID)
    nlClose(rsocket);
  if (usocket != NL_INVALID)
    nlClose(usocket);
  delete ps;
}

//##ModelId=3B0753810078
PacketStream& Connection::stream() {
  return *ps;
}

//##ModelId=3B0753810079
Connection::Stats Connection::getStats() const {
  return Stats();
}

//##ModelId=3B075381007B
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

//##ModelId=3B075381007E
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

//##ModelId=3B0753810081
bool Connection::isConnected() const {
  return connected;
}

//##ModelId=3B0753810083
void Connection::disconnect() {
  if (rsocket != NL_INVALID)
    nlClose(rsocket);
  if (usocket != NL_INVALID)
    nlClose(usocket);
}

//##ModelId=3B0753810084
void Connection::disconnectSendAll() {
	ps->waitToSendAll();
	disconnect();
}

//##ModelId=3B0753810085
void Connection::onFailure(FailureType errorType) {
}

//##ModelId=3B07538100AC
void Connection::onReceive() {
}

//##ModelId=3B07538100AE
void Connection::onDoneWriting() {
}

//##ModelId=3B07538100B0
void Connection::onReceive(bool reliable) {
	//Create buffer into a RawPacket
	NLbyte* buf = new NLbyte[RawPacket::RAW_PACKET_LEN];
	rawRead(reliable, buf, RawPacket::RAW_PACKET_LEN);
	RawPacket raw(buf);

	//parse the packets and add them to the PacketStream
	bool dummy;
	Packet* next = NULL;
	while ((next = PacketParser::parseNextPacket(dummy, raw)) != NULL) {
		ps->addIncomingPacket(next);
	}
	delete[] buf;

	//Start the event
  onReceive();
}

//##ModelId=3B075381004E
Connection::ConnectionListener::ConnectionListener(Connection& listener, bool isReliable) 
: conn(listener), reliable(isReliable) {
}

//##ModelId=3B0753810051
Connection::ConnectionListener::~ConnectionListener() {
}

//##ModelId=3B0753810053
void Connection::ConnectionListener::onReceive() {
  conn.onReceive(reliable);
}

//##ModelId=3B6E14AC0104
void Connection::reg(bool reliable, bool unreliable) {
	if (rlistener == NULL) {
		rlistener = new ConnectionListener(*this, true);
		eGen->reg(rsocket, rlistener);
	}
	if (ulistener == NULL) {
		ulistener = new ConnectionListener(*this, false);
		eGen->reg(usocket, ulistener);
	}
}

//##ModelId=3B6E14AC01D6
void Connection::unreg(bool reliable, bool unreliable) {
	if (rlistener != NULL) {
		eGen->unreg(rsocket);
		delete rlistener;
		rlistener = NULL;
	}
	if (ulistener != NULL) {
		eGen->unreg(usocket);
		delete ulistener;
		ulistener = NULL;
	}
}

//##ModelId=3B6B302400CA
int Connection::rawRead(bool reliable, const NLbyte* buf, int bufSize) {
  NLsocket act;
  if (reliable)
    act = rsocket;
  else
    act = usocket;
  return int(nlRead(act, (NLvoid*)buf, (NLint)bufSize));
}

/**
 * \todo if an error occurs in the rawWrite/Read functions, call onFailure.
 */
//##ModelId=3B6B302401D6
int Connection::rawWrite(bool reliable, const NLbyte* buf, int bufSize) {
  NLsocket act;
  if (reliable)
    act = rsocket;
  else
    act = usocket;
  return int(nlWrite(act, (NLvoid*)buf, (NLint)bufSize));
}

}


