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
#include "../include/gnelib/ConnectionParams.h"
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/ServerConnectionListener.h"
#include "../include/gnelib/Address.h"
#include "../include/gnelib/EventThread.h"
#include "../include/gnelib/RawPacket.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Errors.h"
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
  //##ModelId=3CBE05D50012
  ConnectionParams cp;
  //##ModelId=3C65C6D000F9
  ServerConnectionListener* creator;
  //##ModelId=3C7059BF0112
  bool doJoin;
};

//##ModelId=3B075381027A
ServerConnection::ServerConnection(const ConnectionParams& p,
                                   NLsocket rsocket2,
                                   ServerConnectionListener* creator)
: Thread("SrvrConn", Thread::HIGH_PRI) {
  gnedbgo(5, "created");
  assert(!p);
  sockets.r = rsocket2;
  setListener(p.getListener());
  setTimeout(p.getTimeout());
  params = new ServerConnectionParams;
  params->cp = p;
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
  assert(getListener() != NULL);
  //endConnect will set the null listener to discard the events, so we
  //have to cache the current listener.
  ConnectionListener* origListener = getListener();

  //We cache the remote address because after an error occurs we may not be
  //able to get it to pass to the listen failure function.
  Address rAddr = getRemoteAddress(true);
  gnedbgo1(1, "New connection incoming from %s", rAddr.toString().c_str());

  //Do the GNE protocol handshake.
  try {
    doHandshake();
  } catch (Error& e) {
    params->creator->onListenFailure(e, rAddr, origListener);
    //We delete ourselves when we terminate since we were never seen by the
    //user and no one has seen us yet.
    detach(true);
    return;
  }
  gnedbgo(2, "GNE Protocol Handshake Successful.");

  //Start up the connecting SyncConnection and start the onNewConn event.
  bool onNewConnFinished = false;
  SyncConnection sConn(this);
  try {
    sConn.startConnect();
    ps->start();
    connecting = true;
    eventListener->start();
    reg(true, (sockets.u != NL_INVALID));

    gnedbgo2(2, "Starting onNewConn r: %i, u: %i", sockets.r, sockets.u);
    getListener()->onNewConn(sConn); //SyncConnection will relay this
    onNewConnFinished = true;

    params->creator->onListenSuccess(origListener);

    finishedConnecting();

    //Start bringing connection to normal state.  SyncConnection will make
    //sure onDisconnect gets called starting with endConnect().
    sConn.endConnect(true);
    sConn.release();
  } catch (Error& e) {
    if (!onNewConnFinished) {
      sConn.endConnect(false);
      params->creator->onListenFailure(e, rAddr, origListener);
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

//##ModelId=3C783ACF02BE
void ServerConnection::doHandshake() {
  //Receive the client's CRP.
  gnedbgo(2, "Waiting for the client's CRP.");
  try {
    getCRP();
  } catch (Error& e) {
    if (e.getCode() == Error::GNETheirVersionHigh ||
      e.getCode() == Error::GNETheirVersionLow ||
      e.getCode() == Error::UserVersionMismatch ||
      e.getCode() == Error::WrongGame) {
      gnedbgo(2, "Versions don't match, sending refusal.");
      sendRefusal();
    }
    throw;
  }
  
  //Else, we send the CAP
  gnedbgo(2, "Got CRP, now sending CAP.");
  sendCAP();
  
  //Then we handle anything related to the unreliable connection if needed.
  if (params->cp.getUnrel()) {
    gnedbgo(2, "Unreliable requested.  Getting unrel info.");
    getUnreliableInfo();
  } else {
    gnedbgo(2, "Unreliable connection not requested or refused.");
  }
}

const int CRPLEN = 48;

//##ModelId=3C5CED0502CD
void ServerConnection::getCRP() {
  gbyte crpBuf[64];
  int check = sockets.rawRead(true, crpBuf, 64);
  if (check != CRPLEN) {
    if (check == NL_INVALID) {
      gnedbgo(1, "nlRead error when trying to get CRP.");
      throw LowLevelError(Error::Read);
    } else {
      gnedbgo2(1, "Protocol violation trying to get CRP.  Got %d bytes expected %d",
        check, CRPLEN);
      throw ProtocolViolation(ProtocolViolation::InvalidCRP);
    }
  }

  //Now parse the CRP
  RawPacket crp(crpBuf);

  //Check the header and versions.  These will throw exceptions if there is
  //a problem.
  checkHeader(crp, ProtocolViolation::InvalidCRP);
  checkVersions(crp);

  guint32 maxOutRate;
  crp >> maxOutRate;

  gbool unreliable;
  crp >> unreliable;
  //We use the unreliable connection only if both sides allow it.
  params->cp.setUnrel(unreliable && params->cp.getUnrel());

  //Now that we know the versions are OK, make the PacketStream
  ps = new PacketStream(params->cp.getOutRate(), maxOutRate, *this);
}

//##ModelId=3C5CED0502CE
void ServerConnection::sendRefusal() {
  RawPacket ref;
  addHeader(ref);
  ref << gFalse;
  addVersions(ref);

  int check = sockets.rawWrite(true, ref.getData(), (NLint)ref.getPosition());
  //We don't check for error because if there we don't really care since we
  //are refusing the connection there is nothing else we can do, but we will
  //do it just for the logs.
  if (check != ref.getPosition()) {
    gnedbgo1(1, "Writing the refusal packet failed, got a return of %d",
      check);
  }
}

//##ModelId=3C5CED0502D7
void ServerConnection::sendCAP() {
  RawPacket cap;
  addHeader(cap);
  cap << gTrue;
  cap << params->cp.getInRate();
  if (params->cp.getUnrel()) {
    //If the client requested it and we allowed it, open an unreliable port
    //and send the port number to the client.
    sockets.u = nlOpen(0, NL_UNRELIABLE);
    cap << (gint32)sockets.getLocalAddress(false).getPort();
  } else {
    //Send -1 to tell the client there will be no unreliable port
    cap << (gint32)-1;
  }

  int check = sockets.rawWrite(true, cap.getData(), (NLint)cap.getPosition());
  gnedbgo1(5, "Sent a CAP with %d bytes.", check);
  //The write should succeed and have sent all of our data.
  if (check != cap.getPosition())
    throw LowLevelError(Error::Write);
}

//##ModelId=3C5CED0502D8
void ServerConnection::getUnreliableInfo() {
  gbyte* buf = new gbyte[64];
  int check = sockets.rawRead(true, buf, 64);
  if (check != sizeof(gint32)) {
    delete[] buf;
    if (check == NL_INVALID) {
      gnedbgo(1, "nlRead error when trying to get unreliable info.");
      throw LowLevelError(Error::Read);
    } else {
      gnedbgo2(1, "Protocol violation trying to get unrel info.  Got %d bytes expected %d",
        check, sizeof(guint16));
      throw Error(Error::ProtocolViolation);
    }
  }

  RawPacket raw(buf);
  gint32 portNum;
  raw >> portNum;
  if (portNum < 0 || portNum > 65535) {
    gnedbgo1(1, "Protocol violation: invalid port %d received", portNum);
    throw Error(Error::ProtocolViolation);
  }

  Address uDest = sockets.getRemoteAddress(true);
  uDest.setPort((int)portNum);
  NLaddress temp = uDest.getAddress();
  nlSetRemoteAddr(sockets.u, &temp);

  delete[] buf;
}

}
