#ifndef CLIENTCONNECTION_H_INCLUDED_C51A7F46
#define CLIENTCONNECTION_H_INCLUDED_C51A7F46

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
#include "Thread.h"

namespace GNE {

/**
 * A class that can connect to a remote target.  Inherit from this class,
 * overriding the functions from Connection and ClientConnection based on the
 * events you wish to respond to.
 */
//##ModelId=3B0753800270
class ClientConnection : public Connection, public Thread {
public:
  /**
   * Initializes this ClientConnection.
   * @see Connection#Connection(int, int, std::string)
   * @see ClientConnection(int, int NLaddress)
   */
  //##ModelId=3B075380037F
  ClientConnection(int outRate, int inRate);

  //##ModelId=3B07538003B8
  virtual ~ClientConnection();

  /**
   * Don't call this method.  Connection starts a new thread lasting only
   * while it is connecting.
   * @see Thread#run()
   */
  //##ModelId=3B07538003BA
  void run();

  /**
   * Opens the socket, ready for connect, but does not yet connect.
	 * Normally this function does not block, but if you give a hostname that
	 * needs to be resolved, a delay (possibly long) will occur while the
	 * lookup occurs.
   * @param dest an address in the form of xxx.xxx.xxx.xxx:ppppp (for
   *        internet sockets, or an appropriate NL format if not IP).
	 *        You may use a host name rather than an IP address (ex:
	 *        computer.domain.com:7878).\n
	 *        In both cases the port is optional, but if you leave the port
	 *        number out, set remotePort to the value you want.
	 * @param remotePort if not 0, overrides any port that may have been
	 *                   defined by dest for the remote connection.
   * @param localPort local port to open on, default 0 for don't care.
   * @return true if the socket could not be opened.
   */
  //##ModelId=3B07538003BB
  bool open(std::string dest, int remotePort = 0, int LocalPort = 0);

  /**
   * Alternate version of open taking a native HawkNL address.
   * @param dest destination address and port.
   * @param localPort local port to open on, default 0 for don't care.
   * @return true if the socket could not be opened.
   */
  //##ModelId=3B07538003BE
  bool open(NLaddress dest, int localPort = 0);

  /**
   * Starts connection to the specified target.  This method does not block,
   * and a thread will be started to handle the connection process.
   * onConnect() or onConnectFailure() will be called depending on the
   * outcome of this process.\n
   * You must call either detach(false) or join on this object after calling
   * connect, depending if you want to wait for the connection to complete.\n
   * You can call join after connect to wait until the connection is
   * finished, when onConnect or onConnectFailure will be called.  When the
   * called function exits, the thread will stop and join will return.  If
   * you do not need asyncronous sockets, you do not need to overload these
   * functions and can rely on the isConnection function.  You still need to
   * overload onConnectFailure to get a description of the error, however.\n
   * The version number for the library and your own version number will be
   * checked during this phase.  If either protocol versions mismatch,
   * onConnectFailure() will be triggered.
   * @see #onConnect()
   * @see #onConnectFailure()
   */
  //##ModelId=3B07538003C1
  void connect();

  /**
   * Event triggered after there is a successful connection.  The connection
	 * process will not be considered complete until this function completes.
   */
  //##ModelId=3B07538003C2
  virtual void onConnect() = 0;

  /**
   * Event triggered when a connection failed.  Note that if
   * you are joining on this object, the calling thread will still block
   * until this function returns.\n
	 * After a connection failure, the connection is as if disconnect() was
	 * called, therefore you cannot reconnect this connection instance.
	 * @see Connection::disconnect()
   */
  //##ModelId=3B07538003C4
  virtual void onConnectFailure(const Error& error) = 0;

private:
  /**
   * Address used only while connecting, then unused afterwards.
   */
  NLaddress address;
};

}
#endif /* CLIENTCONNECTION_H_INCLUDED_C51A7F46 */



