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
/*
	  enum FailureType {
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
*/

//##ModelId=3AE5BA8F038E
const std::string Connection::FailureStrings[] = {
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

//##ModelId=3B0753810073
Connection::Connection(int outRate, int inRate)
: rsocket(NL_INVALID), usocket(NL_INVALID), connected(false), 
rlistener(NULL), ulistener(NULL) {
	ps = new PacketStream(outRate, inRate, *this);
}

//##ModelId=3B0753810076
Connection::~Connection() {
	disconnect();
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

/**
 * \bug I think because of the PacketStream thread, if you disconnect, you
 *      cannot reconnect because you cannot restart threads.  If the Thread
 *      class is restartable, then this will be fine.
 */
//##ModelId=3B0753810083
void Connection::disconnect() {
	if (connected) {
		//This is nessacary because we can't join on ps if it has already been
		//  shutdown and/or never started
		unreg(true, true);
		gnedbgo2(2, "disconnecting r: %i, u: %i", rsocket, usocket);
		ps->shutDown();
		ps->join();
		if (rsocket != NL_INVALID) {
			nlClose(rsocket);
			rsocket = NL_INVALID;
		}
		if (usocket != NL_INVALID) {
			nlClose(usocket);
			rsocket = NL_INVALID;
		}
		connected = false;
	}
}

//##ModelId=3B0753810084
void Connection::disconnectSendAll() {
	ps->waitToSendAll();
	disconnect();
}

//##ModelId=3B0753810085
void Connection::onFailure(FailureType errorType) {
	gnedbgo1(1, "onFailure Event: %s", FailureStrings[errorType].c_str());
	disconnect();
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
	int temp = rawRead(reliable, buf, RawPacket::RAW_PACKET_LEN);
	if (temp == NL_INVALID) {
		if (nlGetError() == NL_MESSAGE_END) {
			//in HawkNL 1.4b4 and later, this means that the connection was
			//closed on the network-level because the client disconnected or
			//has dropped.  Since we didn't get an "exit" packet, it's an error.
			onFailure(ConnectionDropped);
		} else {
			//This is some other bad error that we need to report
			reportHawkNLErroro();
			onFailure(Read);
		}
	} else if (temp == 0) {
		//In HawkNL 1.4b3 and earlier, this _USED_ to mean that...
		//This means the connection was closed on the network-level because
		//remote computer has purposely closed or has dropped.
		onFailure(ConnectionDropped);
	} else {
		RawPacket raw(buf);
		
		//parse the packets and add them to the PacketStream
		bool errorCheck;
		Packet* next = NULL;
		while ((next = PacketParser::parseNextPacket(errorCheck, raw)) != NULL) {
			ps->addIncomingPacket(next);
		}
		delete[] buf;
		
		//Start the event
		if (errorCheck == false) {
			//These are level 4 since the explicit event log is generated in onFailure
			gnedbgo1(4, "Unknown packet encountered in a message that has %i bytes", temp);
			gnedbgo2(4, "First bytes are %i and %i", (int)buf[0], (int) buf[1]);
			onFailure(UnknownPacket);
		} else {
			gnedbgo1(4, "onReceive event triggered, %i bytes recv", temp);
			onReceive();
		}
	}
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
	if (reliable && rlistener == NULL) {
		assert(rsocket != NL_INVALID);
		rlistener = new ConnectionListener(*this, true);
		eGen->reg(rsocket, rlistener);
		gnedbgo1(3, "Registered reliable socket %i", rsocket);
	}
	if (unreliable && ulistener == NULL) {
		assert(usocket != NL_INVALID);
		ulistener = new ConnectionListener(*this, false);
		eGen->reg(usocket, ulistener);
		gnedbgo1(3, "Registered unreliable socket %i", usocket);
	}
}

//##ModelId=3B6E14AC01D6
void Connection::unreg(bool reliable, bool unreliable) {
	if (reliable && rlistener != NULL) {
		eGen->unreg(rsocket);
		delete rlistener;
		rlistener = NULL;
		gnedbgo1(3, "Unregistered reliable socket %i", rsocket);
	}
	if (unreliable && ulistener != NULL) {
		eGen->unreg(usocket);
		delete ulistener;
		ulistener = NULL;
		gnedbgo1(3, "Unregistered unreliable socket %i", usocket);
	}
}

//##ModelId=3B6B302400CA
int Connection::rawRead(bool reliable, const NLbyte* buf, int bufSize) {
  NLsocket act;
  if (reliable)
    act = rsocket;
  else
    act = usocket;
	assert(act != NL_INVALID);
  return int(nlRead(act, (NLvoid*)buf, (NLint)bufSize));
}

//##ModelId=3B6B302401D6
int Connection::rawWrite(bool reliable, const NLbyte* buf, int bufSize) {
  NLsocket act;
  if (reliable)
    act = rsocket;
  else
    act = usocket;
	assert(act != NL_INVALID);
	gnedbgo2(1, "First bytes are %i and %i", (int)buf[0], (int) buf[1]);
  return int(nlWrite(act, (NLvoid*)buf, (NLint)bufSize));
}

}


