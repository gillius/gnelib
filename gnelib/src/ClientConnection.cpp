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
#include "../include/gnelib/GNE.h"
#include "../include/gnelib/ClientConnection.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/EventThread.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/PacketParser.h"

namespace GNE {

/**
 * Simply just a class to temporarily hold connection parameters until the
 * ClientConnection is connected, then it is useless.
 */
//##ModelId=3C5CED0500C7
class ClientConnectionParams {
public:
  //##ModelId=3C65C6D0001D
  Address dest;
  //##ModelId=3C5CED050187
  int outRate;
  //##ModelId=3C5CED050188
  int inRate;
  //##ModelId=3C5CED050189
  bool unrel;
  //##ModelId=3C65C6D00029
  SyncConnection* sConnPtr;
};

//##ModelId=3B075380037F
ClientConnection::ClientConnection(ConnectionListener* listener)
  : Connection(listener), Thread("CliConn"), params(NULL) {
  gnedbgo(5, "created");
}

  //##ModelId=3B07538003B8
ClientConnection::~ClientConnection() {
  delete params;
  gnedbgo(5, "destroyed");
}

//##ModelId=3B07538003BB
bool ClientConnection::open(const Address& dest, int outRate, int inRate,
                            int localPort, bool wantUnreliable) {
  if (!dest || outRate < 0 || inRate < 0 || localPort < 0 ||
      localPort > 65535)
    return true;
  else {
    params = new ClientConnectionParams;
    params->dest = dest;
    params->inRate = inRate;
    params->outRate = outRate;
    params->unrel = wantUnreliable;

    sockets.r = nlOpen(localPort, NL_RELIABLE_PACKETS);
  }
  return (sockets.r == NL_INVALID);
}

//##ModelId=3B07538003C1
void ClientConnection::connect(SyncConnection* wrapped) {
  assert(sockets.r != NL_INVALID);
  assert(params);
  assert(params->dest);
  params->sConnPtr = wrapped;
  start();
}

//##ModelId=3B07538003BA
void ClientConnection::run() {
  gnedbgo1(1, "Trying to connect to %s", params->dest.toString().c_str());

  NLaddress temp = params->dest.getAddress();
  NLboolean check = nlConnect(sockets.r, &temp);

  if (check == NL_TRUE) {
    assert(eventListener != NULL);
    //Try to connect using the GNE protocol before we mess with any of the
    //user stuff.
    try {
      doHandshake();
    } catch (Error e) {
      gnedbgo1(1, "Connection failure during GNE handshake: %s", e.toString().c_str());
      getListener()->onConnectFailure(e);
      return;
    }
    gnedbgo(2, "GNE Protocol Handshake Successful.");

    //We don't want to doubly-wrap SyncConnections, so we check for a wrapped
    //one here and else make our own.
    bool ourSConn = false;
    if (!params->sConnPtr) {
      params->sConnPtr = new SyncConnection(this);
      ourSConn = true;
    } else
      assert(params->sConnPtr == getListener());
    //The sConn reference variable is used only for syntactical convienence.
    SyncConnection& sConn = *params->sConnPtr;
    
    bool onConnectFinished = false;
    try {
      //We only want to hold events on our own SyncConnection.  On a user
      //supplied SyncConnection, when it fails we will fail, and
      //SyncConnection::connect() will throw an error.
      if (ourSConn)
        sConn.startConnect();
      ps->start();
      connecting = true;
      eventListener->start();
      reg(true, (sockets.u != NL_INVALID));

      gnedbgo2(2, "Starting onConnect r: %i, u: %i", sockets.r, sockets.u);
      getListener()->onConnect(sConn); //SyncConnection will relay this
      onConnectFinished = true;
      finishedConnecting();

      if (ourSConn) {
        sConn.endConnect(true);
        sConn.release();
        delete params->sConnPtr;
      }

      //We are done using the ClientConnectionParams.
      delete params;
      params = NULL;

    } catch (Error e) {
      if (!onConnectFinished) {
        //endConnect will set the null listener to discard the events, so we
        //have to cache the current listener.
        ConnectionListener* cached = getListener();
        
        if (ourSConn)
          sConn.endConnect(false);
        
        cached->onConnectFailure(e);
      }
      if (ourSConn)
        delete params->sConnPtr;

      //We are done using the ClientConnectionParams.
      delete params;
      params = NULL;
    }
  } else {
    assert(eventListener != NULL);
    Error err = Error::createLowLevelError(Error::ConnectionTimeOut);
    gnedbgo1(1, "Connection failure: %s", err.toString().c_str());
    getListener()->onConnectFailure(err);
  }
}

//##ModelId=3C783ACF0192
void ClientConnection::doHandshake() throw (Error) {
  //Start the GNE protocol connection process.
  //The first packet is from client to server, and is the connection
  //request packet (CRP).
  gnedbgo(2, "Sending the CRP.");
  sendCRP();
  
  //Now we expect to receive the connection accepted packet (CAP) or the
  //refused connection packet, and then based on that set up the
  //unreliable connection.
  gnedbgo(2, "Waiting for the CAP.");
  Address temp = getCAP();
  
  if (params->unrel) {
    gnedbgo(2, "Setting up the unreliable connection");
    setupUnreliable(temp);
  } else {
    gnedbgo(2, "Unreliable connection not requested.");
  }
}

//##ModelId=3C5CED05016E
void ClientConnection::sendCRP() throw (Error) {
  GNEProtocolVersionNumber ver = GNE::getGNEProtocolVersion();

  RawPacket crp;
  crp << ver.version << ver.subVersion << ver.build;
  crp << GNE::getUserVersion();
  crp << (guint32)params->inRate;
  crp << ((params->unrel) ? gTrue : gFalse);

  int check = sockets.rawWrite(true, crp.getData(), crp.getPosition());
  //The write should succeed and have sent all of our data.
  if (check != crp.getPosition())
    throw Error::createLowLevelError(Error::Write);
}

const int REFLEN = sizeof(gbool) + sizeof(guint8) + sizeof(guint8) +
                   sizeof(guint16) + sizeof(guint32);

const int CAPLEN = sizeof(gbool) + sizeof(guint32);

//##ModelId=3C5CED05016F
Address ClientConnection::getCAP() throw (Error) {
  gbyte* capBuf = new gbyte[RawPacket::RAW_PACKET_LEN];
  int check = sockets.rawRead(true, capBuf, RawPacket::RAW_PACKET_LEN);
  if (check == NL_INVALID) {
    delete[] capBuf;
    throw Error::createLowLevelError(Error::Read);
  }

  //The packet must be at least as large to check if it is a CAP or a
  //refusal packet.
  if (check < (int)sizeof(gbool)) {
    delete[] capBuf;
    gnedbgo1(1, "Response to CRP is too small (size is %d).", check);
    throw Error(Error::ProtocolViolation);
  }

  //Now parse the CAP (or refusal packet)
  RawPacket cap(capBuf);
  gbool isCAP;
  cap >> isCAP;

  if (isCAP) {
    //Check to make sure packet sizes match.
    //The size should be CAPLEN if we are not expecting the unreliable info.
    //but if we are expecting it we'll get another guint16.
    int expecting = CAPLEN;
    if (params->unrel)
      expecting += (int)sizeof(guint16);

    if (check != expecting){
      delete[] capBuf;
      gnedbgo2(1, "Expected a CAP of size %d but got %d bytes instead.",
        expecting, check);
      throw Error(Error::ProtocolViolation);
    }

    //Get the max rate the remote end allows.
    guint32 maxOutRate;
    cap >> maxOutRate;

    //Now we have enough info to create our PacketStream.
    ps = new PacketStream(params->outRate, maxOutRate, *this);

    Address ret = params->dest;
    if (params->unrel) {
      //Get the unreliable connection information
      guint16 portNum;
      cap >> portNum;
      ret.setPort((int)portNum);
    }
    //else the result of the returned Address is undefined.

    delete[] capBuf;
    return ret;
  }
  //else we got a refusal packet
  if (check != REFLEN) {
    delete[] capBuf;
    gnedbgo2(1, "Expected a refusal of size %d but got %d bytes instead.",
      REFLEN, check);
    throw Error(Error::ProtocolViolation);
  }

  //Get the version numbers
  GNEProtocolVersionNumber them;
  cap >> them.version >> them.subVersion >> them.build;

  guint32 themUser;
  cap >> themUser;

  //We are done with the CAP packet
  delete[] capBuf;

  //Check the GNE version numbers.  This will throw the right exception if
  //the versions do not match.
  GNE::checkVersions(them, themUser);

  //If the version numbers are all the same our connection was simply
  //just refused.
  throw Error(Error::ConnectionRefused);

  //We should never reach this point.
  assert(false);
  return Address();
}

//##ModelId=3C5CED050170
void ClientConnection::setupUnreliable(const Address& dest) throw (Error) {
  assert(dest);
  sockets.u = nlOpen(0, NL_UNRELIABLE);
  if (sockets.u == NL_INVALID)
    throw Error::createLowLevelError(Error::CouldNotOpenSocket);
  NLaddress temp = dest.getAddress();
  nlSetRemoteAddr(sockets.u, &temp);

  //Now send back our local info, and send a dummy packet out first to open
  //up any possible firewalls or gateways.
  int check = 0;

  guint16 ourPort = (guint16)sockets.getLocalAddress(false).getPort();
  RawPacket resp;
  resp << ourPort;
  check = sockets.rawWrite(true, resp.getData(), resp.getPosition());
  if (check != resp.getPosition())
    throw Error::createLowLevelError(Error::Write);

  resp.reset();
  resp << PacketParser::END_OF_PACKET;
  check = sockets.rawWrite(false, resp.getData(), resp.getPosition());
  if (check != resp.getPosition())
    throw Error::createLowLevelError(Error::Write);
}

}
