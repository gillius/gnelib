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
#include "../include/gnelib/ServerConnection.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/ServerConnectionListener.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/EventThread.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/SocketPair.h"
#include "../include/gnelib/GNE.h"

namespace GNE {

/**
 * Simply just a class to temporarily hold connection parameters until the
 * ClientConnection is connected, then it is useless.
 */
//##ModelId=3C5CED05010A
class ServerConnectionParams {
public:
  //##ModelId=3C5CED0502EB
  int outRate;
  //##ModelId=3C5CED0502EC
  int inRate;
  //##ModelId=3C65C6D000F9
  ServerConnectionListener* creator;
  //##ModelId=3C5CED0502F2
  bool unrel;
  bool doJoin;
};

//##ModelId=3B075381027A
ServerConnection::ServerConnection(int outRate, int inRate, 
                                   ConnectionListener* listener, 
                                   NLsocket rsocket2, 
                                   ServerConnectionListener* creator)
: Connection(listener), Thread("SrvrConn") {
  gnedbgo(5, "created");
  sockets.r = rsocket2;
  params = new ServerConnectionParams;
  params->outRate = outRate;
  params->inRate = inRate;
  params->creator = creator;
  params->doJoin = true;
}

//##ModelId=3B075381027E
ServerConnection::~ServerConnection() {
  //If this is the current thread, we did a detach(true) because we failed 
  //before onNewConn.  Else this is another thread (likely our event thread)
  //and therefore we call join to wait for run to complete if it is still
  //running (as it would be if an error occured after onNewConn but before
  //run finished), and join will also release thread resources.
  //We have to rely on params because Thread::currentThread is not valid in
  //the dtor when we do detach(true).  When an error occurs, params was not
  //deleted, so if it was deleted we know we did not call detach(true) because
  //we did not catch an error.
  if (params == NULL)
    join();
  delete params;
  gnedbgo(5, "destroyed");
}

//##ModelId=3B0753810280
void ServerConnection::run() {
  assert(sockets.r != NL_INVALID);
  assert(eventListener != NULL);
  gnedbgo1(1, "New connection incoming from %s", getRemoteAddress(true).toString().c_str());

  //Do the GNE protocol handshake.
  try {
    //Receive the client's CRP.
    gnedbgo(4, "Waiting for the client's CRP.");
    Error err = getCRP();

    if (err.getCode() == Error::GNETheirVersionHigh ||
        err.getCode() == Error::GNETheirVersionLow ||
        err.getCode() == Error::UserVersionMismatch) {
      gnedbgo(4, "Versions don't match, sending refusal.");
      sendRefusal();
      throw err;
    }
    
    //Else, we send the CAP
    gnedbgo(4, "Got CRP, now sending CAP.");
    sendCAP();

    //Then we handle anything related to the unreliable connection if needed.
    if (params->unrel) {
      gnedbgo(4, "Unreliable requested.  Getting unrel info.");
      getUnreliableInfo();
    } else
      gnedbgo(4, "Unreliable connection not requested.");

  } catch (Error e) {
    params->creator->onListenFailure(e, getRemoteAddress(true), getListener());
    //We delete ourselves when we terminate since we were never seen by the
    //user and no one has seen us yet.
    detach(true);
    return;
  }
  gnedbgo(4, "GNE Protocol Handshake Successful.");

  bool onNewConnFinished = false;
  SyncConnection sConn(this);
  try {
    sConn.startConnect();
    ps->start();
    connecting = true;
    eventListener->start();
    reg(true, (sockets.u != NL_INVALID));

    //Do GNE protocol connection negotiaion here

    gnedbgo2(2, "Starting onNewConn r: %i, u: %i", sockets.r, sockets.u);
    getListener()->onNewConn(sConn); //SyncConnection will relay this
    onNewConnFinished = true;
    finishedConnecting();

    //Start bringing connection to normal state.  SyncConnection will make
    //sure onDisconnect gets called starting with endConnect().
    sConn.endConnect(true);
    sConn.release();
  } catch (Error e) {
    if (!onNewConnFinished) {
      sConn.endConnect(false);
      params->creator->onListenFailure(e, getRemoteAddress(true), getListener());
      //We delete ourselves when we terminate since we were never seen by the
      //user and no one has seen us yet.
      detach(true);
      return;
    }
  }

  //If there were no errors, we delete our connection params:
  delete params;
  params = NULL;
}

const int CRPLEN = sizeof(guint8) + sizeof(guint8) + sizeof(guint16) +
                   sizeof(guint32) + sizeof(guint32) + sizeof(gbool);

//##ModelId=3C5CED0502CD
Error ServerConnection::getCRP() {
  gbyte* crpBuf = new gbyte[RawPacket::RAW_PACKET_LEN];
  int check = sockets.rawRead(true, crpBuf, RawPacket::RAW_PACKET_LEN);
  if (check != CRPLEN) {
    delete[] crpBuf;
    if (check == NL_INVALID) {
      gnedbgo(1, "nlRead error when trying to get CRP.");
      throw Error::createLowLevelError(Error::Read);
    } else {
      gnedbgo2(1, "Protocol violation trying to get CRP.  Got %d bytes expected %d",
        check, CRPLEN);
      throw Error::createLowLevelError(Error::ProtocolViolation);
    }
  }

  //Now parse the CRP
  RawPacket crp(crpBuf);

  //Get the version numbers
  GNEProtocolVersionNumber them;
  crp >> them.version >> them.subVersion >> them.build;

  guint32 themUser;
  crp >> themUser;

  guint32 maxOutRate;
  crp >> maxOutRate;

  gbool unreliable;
  crp >> unreliable;
  params->unrel = (unreliable != 0);

  //We are done with the CRP packet
  delete[] crpBuf;

  try {
    GNE::checkVersions(them, themUser);
  } catch (Error e) {
    return e;
  }

  //Now that we know the versions are OK, make the PacketStream
  if ((int)maxOutRate < params->outRate)
    params->outRate = (int)maxOutRate;
  ps = new PacketStream(params->outRate, params->inRate, *this);

  //We made it through without any errors!
  return Error(Error::NoError);
}

//##ModelId=3C5CED0502CE
void ServerConnection::sendRefusal() {
  GNEProtocolVersionNumber us = GNE::getGNEProtocolVersion();
  RawPacket ref;
  ref << gFalse;
  ref << us.version << us.subVersion << us.build;
  ref << GNE::getUserVersion();

  sockets.rawWrite(true, ref.getData(), (NLint)ref.getPosition());
  //We don't check for error because if there we don't really care since we
  //are refusing the connection and there is nothing else we can do.
}

//##ModelId=3C5CED0502D7
void ServerConnection::sendCAP() throw (Error) {
  RawPacket cap;
  cap << gTrue;
  cap << params->inRate;
  if (params->unrel) {
    //If the client requested it, open an unreliable port and send the port
    //number to the client.
    sockets.u = nlOpen(0, NL_UNRELIABLE);
    cap << (guint16)(sockets.getLocalAddress(false).getPort());
  }

  int check = sockets.rawWrite(true, cap.getData(), (NLint)cap.getPosition());
  //The write should succeed and have sent all of our data.
  if (check != cap.getPosition())
    throw Error::createLowLevelError(Error::Write);
}

//##ModelId=3C5CED0502D8
void ServerConnection::getUnreliableInfo() throw (Error) {
  gbyte* buf = new gbyte[RawPacket::RAW_PACKET_LEN];
  int check = sockets.rawRead(true, buf, RawPacket::RAW_PACKET_LEN);
  if (check != sizeof(guint16)) {
    delete[] buf;
    if (check == NL_INVALID) {
      gnedbgo(1, "nlRead error when trying to get unreliable info.");
      throw Error::createLowLevelError(Error::Read);
    } else {
      gnedbgo2(1, "Protocol violation trying to get unrel info.  Got %d bytes expected %d",
        check, sizeof(guint16));
      throw Error::createLowLevelError(Error::ProtocolViolation);
    }
  }

  RawPacket raw(buf);
  guint16 portNum;
  raw >> portNum;

  Address uDest = sockets.getRemoteAddress(true);
  uDest.setPort((int)portNum);
  nlSetRemoteAddr(sockets.u, &uDest.getAddress());

  delete[] buf;
}

}
