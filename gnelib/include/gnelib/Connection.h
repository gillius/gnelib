#ifndef CONNECTION_H_INCLUDED_C51DC478
#define CONNECTION_H_INCLUDED_C51DC478

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
#include "Thread.h"
#include "PacketStream.h"
#include "ReceiveEventListener.h"
#include "Error.h"
#include "Errors.h"
#include "SocketPair.h"
#include "Address.h"
#include "ConnectionStats.h"

namespace GNE {
class ConnectionListener;
class EventThread;
class SyncConnection;

/**
 * A class resembling any type of connection to a remote computer.  A
 * connection also manages flow control through its PacketStream.
 *
 * When you create a new Connection, you will want to register a
 * ConnectionListener for it so you can receive its events.
 */
//##ModelId=3B075380029E
class Connection {
public:
  /**
   * Intializes this class, given the flow control parameters.
   * @param listener the ConnectionListener instance that will be setup as
   *        the initial listener for the events.  It can be NULL to start
   *        with but must be set before performing any other operations on
   *        this Connection instance.
   */
  //##ModelId=3B0753810073
  Connection(ConnectionListener* listener = NULL);

  /**
   * A Connection will automatically disconnect if it is connected when it
   * is destructed.  Connections won't allocate or deallocate their listeners,
   * so you may want to delete your listener that was registered
   */
  //##ModelId=3B0753810076
  virtual ~Connection();

  /**
   * Returns the currently registered event Listener.  This can be useful to
   * delete your registered user right after disconnection, so you won't have
   * to keep track of the pointer after you allocate it.
   */
  //##ModelId=3BCE75A80280
  ConnectionListener* getListener() const;

  /**
   * Sets a new event listener.  All new events will be sent to the new
   * listener.  The function will block until the current event being
   * processed (by the old listener) completes.  If this is called from the
   * event thread itself the function returns immediately and the next event
   * will be sent to the new listener.
   *
   * If the passed listener is NULL, this will hold the event thread.  This
   * does NOT stop event generation -- generated events still will enter the
   * queue but the actual event function will not be called until a proper
   * listener is set.
   *
   * You MUST have an event listener set because the event thread must call
   * its required events before this object can be destroyed.
   *
   * If you care not to receive events because you are using SyncConnection,
   * you will probably want to set ConnectionListener::getNullListener as the
   * listener to discard all events before you wrap this Connection with a
   * SyncConnection.
   */
  //##ModelId=3BCE75A80282
  void setListener(ConnectionListener* listener);

  /**
   * Returns the PacketStream associated with this class.  Writing and
   * reading through this Connection is done through this PacketStream.
   */
  //##ModelId=3B0753810078
  PacketStream& stream();

  /**
   * If stats is enabled, returns Connection stats.
   * @param reliable <ul>
   *                 <li>If greater than 0, returns reliable stats only.</li>
   *                 <li>If 0, returns unreliable transfer stats only.</li>
   *                 <li>If less than 0, returns cumulative stats for
   *                     both.</li></ul>
   * @return various connection stats.
   * @see ConnectionStats
   * @see GNE::enableStats
   */
  //##ModelId=3B0753810079
  ConnectionStats getStats(int reliable) const;

  /**
   * Returns the local address of this connection.  If the requested socket
   * has not been opened, the returned Address will be invalid (!isValid()).
   * @param reliable sometimes two sockets are used for reliable and
   *                 unreliable data.  Specify which low-level socket you
   *                 want the address of.
   * @see GNE::Address
   */
  //##ModelId=3B075381007B
  Address getLocalAddress(bool reliable) const;

  /**
   * Returns the remote address of this connection.  If the requested socket
   * is not connected, the returned Address will be invalid (!isValid()).
   * @param reliable sometimes two sockets are used for reliable and
   *                 unreliable data.  Specify which low-level socket you
   *                 want the address of.
   * @see GNE::Address
   */
  //##ModelId=3B075381007E
  Address getRemoteAddress(bool reliable) const;

  /**
   * Returns true if this Connection is active and ready to send/recieve.
   */
  //##ModelId=3B0753810081
  bool isConnected() const;

  /**
   * Immediately disconnects this socket.  No more data will be recieved or
   * sent on this socket.  If you want to disconnect more nicely, use
   * disconnectSendAll.  It is okay to call this function even when this
   * Connection is already disconnected.\n
   * NOTE: You may not reconnect this connection object after calling
   * disconnect.
   * @see #disconnectSendAll()
   */
  //##ModelId=3B0753810083
  void disconnect();

  /**
   * A blocking call that disconnects nicely.  This has the same effect as
   * disconnect, except that all data waiting to be sent will be sent if
   * possible.  If the connection is already disconnected, this function does
   * nothing.
   * @param waitTime the max amount of time in ms to wait for the outgoing
   *        packet queue to clear.
   * @see #disconnect()
   */
  //##ModelId=3B0753810084
  void disconnectSendAll(int waitTime = 10000);

protected:
  EventThread* eventListener;

  /**
   * A utility function for ServerConnection and ClientConnection to add the
   * standard header onto the connection packets.
   */
  //##ModelId=3C82ADDA0093
  void addHeader(RawPacket& raw);

  /**
   * A utility function for ServerConnection and ClientConnection to add the
   * version information to a packet.
   */
  //##ModelId=3C82ADDA00A7
  void addVersions(RawPacket& raw);

  /**
   * A utility function for ServerConnection and ClientConnection to check to
   * verify the %GNE header of the connecting packets.  The RawPacket is read
   * from so this function will move the reading position to the next element
   * after the header.  If it throws an exception, the read position is
   * undefined.
   *
   * @param t the type of ProtocolViolation to throw if it throws one.
   * @throw ProtocolViolation if the next bytes of the RawPacket is not the
   *                          proper %GNE header.
   */
  //##ModelId=3C82ABA50326
  void checkHeader(RawPacket& raw,
    ProtocolViolation::ViolationType t = ProtocolViolation::OtherViolation);

  /**
   * A utility function for ServerConnection and ClientConnection to check
   * the GNE version, game name, and user versions in the RawPacket.  This
   * function behaves similarly to checkHeader.
   *
   * @throw Error with a code of Error::GNETheirVersionHigh or
   *   Error::GNETheirVersionLow if the GNE protocol versions do not match.
   * @throw WrongGame if the game names don't match.
   * @throw UserVersionMismatch if the user version numbers don't match.
   */
  //##ModelId=3C82ABA5036C
  void checkVersions(RawPacket& raw);

  //For information about events, see the ConnectionListener class.
  //##ModelId=3B07538100AC
  void onReceive();

  //##ModelId=3B07538100AE
  void onDoneWriting();

  /**
   * The pair of sockets.
   */
  //##ModelId=3BB2CB410263
  SocketPair sockets;

  /**
   * The PacketStream associated with this Connection.  This object also
   * contains information about the in and out connection rates.
   */
  //##ModelId=3B075381006E
  PacketStream* ps;

  /**
   * The connecting has just finished and the flags need to be changed.
   * Since this needs a mutex, we make a function to handle this.
   */
  //##ModelId=3C30E3FF01DE
  void finishedConnecting();

  /**
   * Is this connecting?  We need to know this so we don't report as ready
   * for communications, but still want to disconnect.  This will be set
   * after the sockets are registered and the PacketStream is active.
   */
  //##ModelId=3BDB10A302F8
  bool connecting;

  /**
   * Is this Connection active and ready for communications?
   */
  //##ModelId=3B0753810072
  bool connected;

  /**
   * Register this Connection object's sockets with the
   * ConnectionEventGenerator.  Pass true to register each socket type.
   * Usually both will be registered, but in the future there may exist a
   * new connection type that only uses one of the sockets.\n
   * The internal class ConnectionListener will be used to trigger the proper
   * onReceive(bool) event.\n
   * The sockets will be unregistered automatically when the object is
   * destroyed, but they may be unregistered explicitly at an earlier time
   * through unreg(bool, bool).\n
   * Registering a socket already registered will have no effect.
   * @see #onReceive(bool)
   * @see #unreg(bool, bool)
   */
  //##ModelId=3B6E14AC0104
  void reg(bool reliable, bool unreliable);

  /**
   * Unregistered the sockets which are specified by passing true to the
   * appropriate parameter.  This function call is optional -- when the
   * object is destroyed, unregistering is automatic if socket was
   * registered.  Trying to unregister sockets that are already unregistered
   * will result in no effect.
   */
  //##ModelId=3B6E14AC01D6
  void unreg(bool reliable, bool unreliable);

private:
  //##ModelId=3B07538002A0
  class Listener : public ReceiveEventListener {
  public:
    //##ModelId=3B075381004E
    Listener(Connection& listener, bool isReliable);

    //##ModelId=3B0753810051
    virtual ~Listener();

    //##ModelId=3B0753810053
    void onReceive();

  private:
    //##ModelId=3B0753810049
    Connection& conn;

    //##ModelId=3B075381004D
    bool reliable;

  };

  /**
   * Make Listener a friend so it can call our onRecieve(bool)
   * event, which will properly parse the packets.
   */
  friend class Listener;

  /**
   * Used to keep some functions thread-safe.
   */
  //##ModelId=3BC3B98001C3
  Mutex sync;
  //Used for reg and unreg functions.
  //##ModelId=3C46133F02D5
  Mutex regSync;

  //Possible ReceiveEventListeners that may or may not exist, but if they do
  //we need to kill them on exit.
  //##ModelId=3B6E14AC00FB
  Listener* rlistener;
  //##ModelId=3B6E14AC0100
  Listener* ulistener;

  //PacketStream might call our processError function.
  friend class PacketStream;

  //SyncConnection might call our event functions.
  friend class SyncConnection;

  /**
   * Parses the packets recieved, then calls onReceive.
   */
  //##ModelId=3B07538100B0
  void onReceive(bool reliable);

  /**
   * When we get an ExitPacket we want to ignore any further errors,
   * espically NL_MESSAGE_END.
   */
  //##ModelId=3C73170D0055
  bool exiting;

  /**
   * Determines whether the error given is fatal or non-fatal, and calls the
   * appropriate event, and handles disconnects if necessary.
   */
  //##ModelId=3BB4208C0280
  void processError(const Error& error);
};

}
#endif /* CONNECTION_H_INCLUDED_C51DC478 */
