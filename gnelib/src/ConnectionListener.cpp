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
#include "../include/gnelib/Error.h"
#include "../include/gnelib/ConnectionListener.h"

namespace GNE {

//##ModelId=3BCA817C0046
ConnectionListener::~ConnectionListener() {
}

ConnectionListener* ConnectionListener::getNullListener() {
  static ConnectionListener nullListener;
  return &nullListener;
}

//##ModelId=3BCA83910078
void ConnectionListener::onConnect(SyncConnection& conn) throw (Error) {
}

//##ModelId=3BCA83920262
void ConnectionListener::onConnectFailure(const Error& error) {
}

//##ModelId=3BCFAE5900AA
void ConnectionListener::onNewConn(SyncConnection& newConn) throw (Error) {
}

//##ModelId=3BCA83BA02F8
void ConnectionListener::onDisconnect() {
}

//##ModelId=3C70672B0357
void ConnectionListener::onExit() {
}

//##ModelId=3BCA83C600B4
void ConnectionListener::onError(const Error& error) {
}

//##ModelId=3BCA83C803A2
void ConnectionListener::onFailure(const Error& error) {
}

//##ModelId=3BCA83CF0208
void ConnectionListener::onReceive() {
}

//##ModelId=3BCA83D101F4
void ConnectionListener::onDoneWriting() {
}

} //namespace GNE




