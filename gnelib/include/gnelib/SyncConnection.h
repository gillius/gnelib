#ifndef SYNCCONNECTION_H_INCLUDED_C43C0621
#define SYNCCONNECTION_H_INCLUDED_C43C0621

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
#include "Error.h"
#include "ConnectionListener.h"
#include "ConditionVariable.h"
#include "Mutex.h"

namespace GNE {
class Address;
class Connection;
class Packet;
class ClientConnection;
class ServerConnection;

/**
 * A class for performing syncronous connections.  All methods of this class
 * except writing block until completion.  This is useful while
 * connecting or performing trivial connections/communications.  Many times
 * if you want to perform non-game transfers, for example a server-query
 * connection or another simple service it might be simpler to spawn a new
 * thread and run the SyncConnection while the rest of the program runs.\n
 * SyncConnections throw an Error class on an error, so you must use
 * try/catch blocks.  Note that the Error class can't describe everything
 * about an error, so sometimes more specific information is available in the
 * debug logs (enable them with GNE::initDebug).\n
 * All transfers with SyncConnection are reliable (using TCP, SPX, or similar
 * protocol).\n
 * If an error occurs, the connection was terminated.  The underlying
 * connection is disconnected, and this SyncConnection becomes released.\n
 * When you wrap a Connection with this class, the event listener for that
 * Connection is suspended and SyncConnection "takes over" until it is
 * release()d.  So while in syncronous mode you will receive no events
 * through your asyncronous event listener, with the exception of
 * onDisconnect and onConnect.  It is best only to use SyncConnection
 * while connecting, because of possible side-effects when you
 * wrap the Connection.  If there was already data pending that you did not
 * receive in asyncronous mode -- it was not lost, but you will get it from
 * the next packet read.  If you start out with a SyncConnection, then you
 * can be certain no unexpected packets will be arriving.\n
 * See the example exsynchello for more help with the usage of this class.
 */
//##ModelId=3BC3C5B200BE
class SyncConnection : public ConnectionListener {
public:
  /**
   * Creates a new SyncConnection.  Pass in the Connection that you want to
   * wrap.  See the details above for more information.
   */
  //##ModelId=3BC3CB1703B6
  SyncConnection(Connection* target);

  /**
   * Destructs this SyncConnection, calling release() if necessary.  This
   * function is declared as being able to throw an Error object, but this
   * guaranteed not to fail if the SyncConnection is released already, since
   * the only error that can occur is one given by release.
   * @see release()
   */
  //##ModelId=3BC3CB1703B7
  virtual ~SyncConnection() throw (Error);

  /**
   * Returns the underlying Connection.
   */
  //##ModelId=3BDB10A502A8
  Connection* getConnection() const;
  
  /**
   * Just like ClientConnection::open, this will open the port, ready for
   * connection.  If the open failed, an Error is thrown.
   * \nIt is important that the wrapped Connection is a ClientConnection, 
   * otherwise undefined behavior (likely a crash) will result.
   */
  //##ModelId=3BC3CD0902E4
  void open(const Address& dest, int outRate = 0,
            int inRate = 0, int localPort = 0,
            bool wantUnreliable = true) throw (Error);
  
  /**
   * Attempts to connect to the remote side, and waits for the connection to
   * complete, throwing an Error if it was unsuccessful.  Your event listener
   * will receive the onConnect event resulting from this -- it is not
   * repressed just like onDisconnect is not repressed.  Many times though,
   * when using this method you won't have a need for onConnect, but if you
   * do create one, connect will wait until onConnect is finished.
   * \nIt is important that the wrapped Connection is a ClientConnection, 
   * otherwise undefined behavior (likely a crash) will result.
   */
  //##ModelId=3BC3CD090320
  void connect() throw (Error);
  
  /**
   * Disconnects the underlying Connection.  It is best to use this function
   * instead of getConnection()->disconnect() because this will make sure
   * any pending writes will have completed through a call to release().
   */
  //##ModelId=3BC3CD6E02BD
  void disconnect() throw (Error);

  /**
   * Releases this SyncConnection, returning the underlying Connection to its
   * previous event handler.  Once a SyncConnection has been released, then
   * it is essentially in an invalid state and should not be used anymore
   * (with the exception of the dtor, getConnection(), isReleased(), and
   * this function).  This function throws an Error if some underlying
   * pending operations failed since the last call on this SyncConnection.\n
   * If release is called when the SyncConnection is already released, no
   * errors will be thrown.\n
   * onReceive will be called in the original listener after release if
   * necessary, and onDoneWriting will be called after release if any data
   * since writing packets 
   */
  //##ModelId=3BDB10A50316
  void release() throw (Error);
  
  /**
   * Returns true if release() has been called on this SyncConnection, and it
   * is not a valid object to use anymore.
   */
  //##ModelId=3BDB10A50317
  bool isReleased() const;
  
  /**
   * Reads a packet from the connection.  You should provide an already
   * allocated packet whose Packet::readPacket function will be called.
   * There will be type checking performed before this call to make sure the
   * right packet is being read.  If there is a mismatch, an Error is thrown.
   * The passed packet is untouched, and the connection remains connected;
   * however, the data just received (the incorrect packet) is lost.
   * The connection will remain connected in this case.\n
   * An error is also thrown if a socket failure or error occurs since the
   * last interaction with this object.
   */
  //##ModelId=3BC3CFE50014
  SyncConnection& operator >> (Packet& packet) throw (Error);
  
  /**
   * Writes a Packet to the connection by placing it in the outgoing queue.
   * This method actually doesn't block like every other SyncConnection
   * method, but on a write there is no reason to block, since it will not
   * effect the logic of the code.  This allows for packet caching to improve
   * network performance, and allows you to perform reads while the
   * connection is still writing.  All of this is transparent to your logic,
   * though.  release() will block until all writes are completed, and the
   * destructor and disconnect() function call release() if needed.\n
   * An error is thrown if a socket failure or error occurs since the
   * last interaction with this object, or during this interaction.
   */
  //##ModelId=3BC3DBB000AA
  SyncConnection& operator << (const Packet& packet) throw (Error);
  
private:
  /**
   * Not for use for the end-user API.  During connection, events need to be
   * treated a little differently, as well as releasing, because onNewConn or
   * onConnect may not have been called or are still processing, and we don't
   * want to release.  This is used only by the GNE protocol connecting code,
   * and is used there to help guarantee that onNewConn/onConnect is the
   * first event, and onDisconnect the last if onNewConn/onConnect ever
   * finished.  Basically this "delays" the release of this SyncConnection.\n
   * This must be called BEFORE any events can possibly be received (so
   * before registration into ConnectionEventGenerator).
   */
  //##ModelId=3C4116C3023E
  void startConnect();

  /**
   * Complement function with startConnect, this must be called if
   * startConnect was called.  Pass true to passEvents if the connection was
   * successful and onFailure and onDisconnect should be passed onto the old
   * listener.  Pass false if the connection failed and the events should be
   * discarded and not passed on.
   */
  //##ModelId=3C4116C30248
  void endConnect(bool passEvents) throw (Error);

  //Make friends so they can use startConnect and endConnect.
  friend class ServerConnection;
  friend class ClientConnection;
  
  /**
   * The actual releasing functionality, but in a separate function so that
   * calling functions can manipulate the mutexes, which are not necessarily
   * recursive.
   */
  //##ModelId=3C4116C30249
  void doRelease() throw (Error);

  /**
   * The event listeners for SyncConnection that will override the current
   * listener our connection has.
   */
  //##ModelId=3BDB10A50353
  void onNewConn(SyncConnection& newConn) throw (Error);
  //##ModelId=3BDB10A6000A
  void onConnect(SyncConnection& conn) throw (Error);
  //##ModelId=3BDB10A60078
  void onConnectFailure(const Error& error);
  //##ModelId=3BDB10A60122
  void onDisconnect();
  //##ModelId=3BDB10A60154
  void onError(const Error& error);
  //##ModelId=3BDB10A601FE
  void onFailure(const Error& error);
  //##ModelId=3C70672C009C
  void onExit();
  //##ModelId=3BDB10A6029E
  void onReceive();
  //##ModelId=3C1081BC015B
  void onDoneWriting();

  /**
   * A ConditionVariable we can wait on to wait for incoming data.
   * This is also used as a mutex for errors, as waiting for incoming data
   * is also sensitive to if an error happens -- it will stop waiting if the
   * connection failed.
   */
  //##ModelId=3BDB10A501CE
  ConditionVariable recvNotify;

  /**
   * Checks to see if an Error has occured, and if so, throws it.
   */
  //##ModelId=3BDB10A6029F
  void checkError() throw (Error);

  /**
   * Sets the error.
   */
  //##ModelId=3BDB10A602DA
  void setError(const Error& error);

  /**
   * Syncronization for release and connecting events.
   */
  //##ModelId=3BDB10A501D3
  Mutex sync;

  /**
   * The current error state of this connection.  Error::NoError if there is
   * no error.  recvNotify acts to keep this variable from being accessed by
   * multiple threads.
   */
  //##ModelId=3BDB10A501D8
  Error currError;

  /**
   * The underlying Connection.
   */
  //##ModelId=3BDB10A50209
  Connection* conn;

  /**
   * The old listener for asyncronous communications that the Connection just
   * had.  If this is NULL, then this object has been released.
   */
  //##ModelId=3BDB10A5020E
  ConnectionListener* oldListener;

  //##ModelId=3C4116C30220
  volatile bool released;

  /**
   * If this is true, we have an outstanding unexplained onDoneWriting event
   * that will be passed on when we release, as with onReceive.
   */
  //##ModelId=3C1081BC0024
  bool onDoneWritingEvent;

  /**
   * @see setConnectMode
   */
  //##ModelId=3C4116C30234
  volatile bool connectMode;
};

} // namespace GNE

#endif /* SYNCCONNECTION_H_INCLUDED_C43C0621 */
