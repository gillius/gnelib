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
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Mutex.h"
#include "../include/gnelib/ConditionVariable.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/ClientConnection.h"
#include "../include/gnelib/PacketStream.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

//##ModelId=3BC3CB1703B6
SyncConnection::SyncConnection(Connection* target)
: currError(Error::NoError), conn(target),
released(false), onDoneWritingEvent(false), connectMode(false) {
  gnedbgo(5, "created");
  gnedbgo1(2, "Wrapping Connection %x into a SyncConnection.", conn);
  oldListener = conn->getListener();
  conn->setListener(this);
}

//##ModelId=3BC3CB1703B7
SyncConnection::~SyncConnection() throw (Error) {
  release();
  gnedbgo(5, "destroyed");
}

//##ModelId=3BDB10A502A8
Connection* SyncConnection::getConnection() const {
  return conn;
}

/**
 * \todo Check out possibility of a dynamic cast.
 */
//##ModelId=3BC3CD0902E4
void SyncConnection::open(const Address& dest,
                          int outRate, int inRate, 
                          int localPort, bool wantUnreliable) throw (Error) {
  assert(!isReleased());
  if (((ClientConnection*)conn)->open(dest, outRate, inRate, localPort, wantUnreliable))
    throw Error(Error::CouldNotOpenSocket);
}

/**
 * \todo Check out possibility of a dynamic cast.
 */
//##ModelId=3BC3CD090320
void SyncConnection::connect() throw (Error) {
  assert(!isReleased());
  ClientConnection* cli = (ClientConnection*)conn;
  cli->connect(this);
  cli->join();
  checkError();
}

//##ModelId=3BC3CD6E02BD
void SyncConnection::disconnect() throw (Error) {
  release();
  conn->disconnectSendAll();
}

//##ModelId=3C4116C3023E
void SyncConnection::startConnect() {
  assert(!isReleased());
  sync.acquire();
  connectMode = true;
  sync.release();
}

//##ModelId=3C4116C30248
void SyncConnection::endConnect(bool passEvents) throw (Error) {
  assert(connectMode);
  //We use a LockMutex to lock sync so that if an exception occurs, sync will
  //automatically be unlocked.
  LockMutex lock(sync);

  //If the connection failed we need to ignore any further events as
  //onNewConn or onConnect did not successfully finish and that listener
  //shouldn't get onFailure or onDisconnect.
  if (!passEvents)
    oldListener = ConnectionListener::getNullListener();
  connectMode = false;
  doRelease();
}

//##ModelId=3BDB10A50316
void SyncConnection::release() throw (Error) {
  //We use a LockMutex to lock sync so that if an exception occurs, sync will
  //automatically be unlocked.
  LockMutex lock(sync);
  doRelease();
}

//##ModelId=3C4116C30249
void SyncConnection::doRelease() throw (Error) {
  if (!isReleased() && !connectMode) {
    //If we are not already released and we are not holding events
    gnedbgo1(2, "Releasing Connection %x", conn);
    released = true;
    
    //Notify any receivers there was an error, and set release error if there
    //was no error already, otherwise don't change it.
    recvNotify.acquire();
    if (currError.getCode() == Error::NoError) {
      currError.setCode(Error::SyncConnectionReleased);
      recvNotify.broadcast(); //Let people waiting for data get the error.
      recvNotify.release();
    } else {
      recvNotify.broadcast(); //Let people waiting for data get the error.
      recvNotify.release();
      conn->setListener(oldListener);
      throw currError;
    }

    //Notify the old listener for onReceive and onDoneWriting if needed, and
    //there are no errors that invalidated the stream (detected above).
    if (conn->stream().isNextPacket())
      conn->onReceive();
    if (onDoneWritingEvent)
      conn->onDoneWriting();

    //Then we can start the events up again knowing that if there was a
    //failure that onDisconnect will go to the original listener.
    conn->setListener(oldListener);
    //From this point we cannot access conn as once onDisconnect is called
    //conn could be deleted at any time.
  }
}

//##ModelId=3BDB10A50317
bool SyncConnection::isReleased() const {
  return released;
}

/**
 * \todo consider timed waits in the future, but this won't be needed when
 *       the GNE protocol is finished and detects timeouts.
 * \todo This probably could be optimized quite a bit -- each packet gets
 *       copied twice!
 */
//##ModelId=3BC3CFE50014
SyncConnection& SyncConnection::operator >> (Packet& packet) throw (Error) {
  //We have to acquire the mutex now so that an error cannot occur between
  //checkError and our wait.
  recvNotify.acquire();
  try {
    checkError();
    assert(!isReleased());
    while (!conn->stream().isNextPacket()) {
      recvNotify.wait();
      //Check if we were woken up due to an error.
      checkError();
    }
  } catch (...) {
    recvNotify.release();
    throw;
  }
  recvNotify.release();

  //Now that we have some data, do the actual receiving.
  Packet* recv = conn->stream().getNextPacket();
  assert(recv != NULL);  //There had better be some data!
  if (recv->getType() != packet.getType()) {
    gnedbgo2(1, "Packet type mismatch.  Got %d, expected %d.",
                recv->getType(), packet.getType());
    throw Error(Error::PacketTypeMismatch);
  }

  //Copy the packet.
  //The original method was to use operator=() but doing it this way makes
  //things complicated in several ways, including how to overload it, and the
  //fact that you can't copy the children to children w/o another operator.
  //This should be optimized in the future.  This new method will be easier
  //to optimize later to allow better optimizations.
  RawPacket temp;
  recv->writePacket(temp);
  temp.reset();
  NLubyte dummy;
  temp >> dummy; //Skip the ID code.
  packet.readPacket(temp);

  return *this;
}

//##ModelId=3BC3DBB000AA
SyncConnection& SyncConnection::operator << (const Packet& packet) throw (Error) {
  checkError();
  assert(!isReleased());

  onDoneWritingEvent = false;
  conn->stream().writePacket(packet, true);

  return *this;
}

//##ModelId=3BDB10A50353
void SyncConnection::onNewConn(SyncConnection& newConn) throw (Error) {
  //newConn should be this object.  We don't do any double wrapping.
  assert(this == &newConn);
  assert(!isReleased());
  oldListener->onNewConn(newConn);
}

//##ModelId=3BDB10A6000A
void SyncConnection::onConnect(SyncConnection& conn) throw (Error) {
  assert(!isReleased());
  oldListener->onConnect(conn);
}

//##ModelId=3BDB10A60078
void SyncConnection::onConnectFailure(const Error& error) {
  setError(error);
}

//##ModelId=3BDB10A60122
void SyncConnection::onDisconnect() {
  //This should never happen.  An error should occur first, and at that time
  //we are released, and the onDisconnect event should be sent to the
  //original listener.
  assert(false);
}

//##ModelId=3BDB10A60154
void SyncConnection::onError(const Error& error) {
  conn->disconnect();
  //Turn errors on a SyncConnection into a failure.
  onFailure(error);
}

//##ModelId=3BDB10A601FE
void SyncConnection::onFailure(const Error& error) {
  setError(error);
  //Stop the event thread until release properly restarts it.
  conn->setListener(NULL);
}

//##ModelId=3C70672C009C
void SyncConnection::onExit() {
  setError( Error(Error::ExitNoticeReceived) );
  //Stop the event thread until release properly restarts it.
  conn->setListener(NULL);
}

//##ModelId=3BDB10A6029E
void SyncConnection::onReceive() {
  //Notify anyone who is waiting for data to come in (namely operator <<).
  recvNotify.signal();
}

//##ModelId=3C1081BC015B
void SyncConnection::onDoneWriting() {
  //setListener in release acts like a mutex so that this flag being set will
  //never be lost.
  onDoneWritingEvent = true;
}

//##ModelId=3BDB10A6029F
void SyncConnection::checkError() throw (Error) {
  recvNotify.acquire();
  bool isErr = (currError.getCode() != Error::NoError);
  recvNotify.release();

  if (isErr) {
    //We release when we discover the error rather than when we set it
    //because we can't release from the event thread.
    release();
    //We can throw currError (which will make a copy) because once an error
    //is set it does not change.
    throw currError;
  }
}

//##ModelId=3BDB10A602DA
void SyncConnection::setError(const Error& error) {
  //We don't need to lock sync here because events can't occur while release
  //is setting its "released" error since setListener blocks.  We couldn't
  //lock it anyways for the same reason as well.
  recvNotify.acquire();
  if (currError.getCode() == Error::NoError)
    currError = error;
  recvNotify.broadcast(); //Let people waiting for data get the error.
  recvNotify.release();
}

} //namespace GNE
