#ifndef SOCKETS_H_INCLUDED_C44D55A7
#define SOCKETS_H_INCLUDED_C44D55A7

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

namespace GNE {
class Address;

/**
 * A class used to centralize shared elements between Connection and
 * SyncConnection.  The GNE user will not need to use or know about this
 * class.
 */
//##ModelId=3BB2341D01FE
class SocketPair {
public:
	/**
	 * Initializes this object, with optional initial values for the sockets.\n
	 * The value NL_INVALID should always be used for sockets that are not
	 * active.
	 */
  //##ModelId=3BB2348D0348
  SocketPair(NLsocket reliable = NL_INVALID, NLsocket unreliable = NL_INVALID);

	/**
	 * Since this is just a class to contain the sockets, the sockets are not
	 * disconnected on exit.
	 */
  //##ModelId=3BB2349B015E
  ~SocketPair();

	/**
	 * Calls a low-level disconnect (nlClose) on both sockets, if they are not
	 * NL_INVALID.
	 */
  //##ModelId=3BB2351603A2
  void disconnect();

  /**
   * Returns the local address of this connection.  If the requested socket
	 * has not been opened, an Address is returned which is marked invalid.
   * @param reliable socket select
   */
  //##ModelId=3BB2CB41030C
  Address getLocalAddress(bool reliable) const;

  /**
   * Returns the remote address of this connection, if it is connected,
	 * otherwise, an Address is returned which is marked invalid.
   * @param reliable socket select
   */
  //##ModelId=3BB2CB41037A
  Address getRemoteAddress(bool reliable) const;

	/**
	 * Performs a low-level read on a socket.
	 * @param reliable select which socket to perform read on.
	 * @param buf data arrives in this buffer.
	 * @param bufSize the size of buf.
	 */
  //##ModelId=3B6B302400CA
  int rawRead(bool reliable, const NLbyte* buf, int bufSize) const;

	/**
	 * Performs a low-level wrote on a socket.
	 * @param reliable select which socket to perform read on.
	 * @param buf data to be sent.
	 * @param bufSize the size of data to be sent.
	 */
  //##ModelId=3B6B302401D6
  int rawWrite(bool reliable, const NLbyte* buf, int bufSize) const;

	/**
	 * The reliable socket.
	 */
  //##ModelId=3BB2346A0208
  NLsocket r;

	/**
	 * The unreliable socket.
	 */
  //##ModelId=3BB234640280
  NLsocket u;
};

} // namespace GNE



#endif /* SOCKETS_H_INCLUDED_C44D55A7 */
