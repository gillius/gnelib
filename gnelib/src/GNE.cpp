/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@webzone.net)
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
#include "GNE.h"

//##ModelId=3AE26F6A035C
const double GNE::VER = 0.002;

//##ModelId=3AF8D89F00E6
bool GNE::initialized = false;

//##ModelId=3AE4DF7A03AC
const int GNE::UDP_HEADER_SIZE = 28;

//##ModelId=3AE26FC000A0
const std::string GNE::VER_STR = "GNE v0.002 pre-alpha";

//##ModelId=3AFF3E3500FA
const NLenum GNE::NO_NET = 128;

//##ModelId=3AFF6429023A
int GNE::userVersion = 0;

//##ModelId=3AE270BF0078
bool GNE::init(NLenum networkType, int (*atexit_ptr)(void (*func)(void))) {
  if (!initialized) {
    atexit_ptr(shutdown);
    if (networkType != NO_NET) {
      if (nlInit() == NL_FALSE)
        return true;
      if (nlSelectNetwork(networkType) == NL_FALSE)
        return true;
      nlEnable(NL_BLOCKING_IO);
      nlEnable(NL_TCP_NO_DELAY);
      nlDisable(NL_SOCKET_STATS);
      initialized = true; //We need only to set this to true if we are using HawkNL
    }
    return false;
  }
  return false;
}

//##ModelId=3AE270CE033E
void GNE::shutdown() {
  if (initialized) {
    nlShutdown();
    initialized = false;
  }
}

//##ModelId=3AFF79890208
NLaddress GNE::getLocalAddress() {
  assert(initialized);
  NLaddress ret;
  NLsocket temp = nlOpen(0, NL_RELIABLE);
  nlGetLocalAddr(temp, &ret);
  nlClose(temp);
  return ret;
}

//##ModelId=3AEDEDE100AA
int GNE::getBytesRead() {
  assert(initialized);
  return nlGetInteger(NL_BYTES_RECEIVED);
}

//##ModelId=3AE4C988000A
int GNE::getBytesWritten() {
  assert(initialized);
  return nlGetInteger(NL_BYTES_SENT);
}

//##ModelId=3AE4CA1D0122
void GNE::enableStats() {
  assert(initialized);
  nlEnable(NL_SOCKET_STATS);
}

//##ModelId=3AE4CA2803AC
void GNE::disableStats() {
  assert(initialized);
  nlDisable(NL_SOCKET_STATS);
}

//##ModelId=3AFF64290316
void GNE::clearStats() {
  assert(initialized);
  nlClear(NL_ALL_STATS);
}

//##ModelId=3AE4D10803CA
int GNE::getOpenConnections() {
  assert(initialized);
  return nlGetInteger(NL_OPEN_SOCKETS);
}

//##ModelId=3AFF642A000A
void GNE::setUserVersion(int version) {
  assert(initialized);
  userVersion = version;
}


