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
#include "ConnectionEventGenerator.h"
#include "PacketParser.h"
#include "GNE.h"

namespace GNE {
  namespace PacketParser {
    //this is declared here only so the user cannot access it, and the "real"
    //function can do checking on the ID given to it.
    void registerGNEPackets();
  }

int userVersion = 0;
ConnectionEventGenerator* eGen = NULL;

static bool initialized = false;

bool initGNE(NLenum networkType, int (*atexit_ptr)(void (*func)(void))) {
  if (!initialized) {
    atexit_ptr(shutdownGNE);
    if (networkType != NO_NET) {
      if (nlInit() == NL_FALSE)
        return true;
      if (nlSelectNetwork(networkType) == NL_FALSE)
        return true;
      nlEnable(NL_BLOCKING_IO);
      nlEnable(NL_TCP_NO_DELAY);
      nlDisable(NL_SOCKET_STATS);
      PacketParser::registerGNEPackets();
      eGen = new ConnectionEventGenerator();
      eGen->start();
      initialized = true; //We need only to set this to true if we are using HawkNL
    }
    return false;
  }
  return false;
}

void shutdownGNE() {
  if (initialized) {
    eGen->shutdown();
    eGen->join();
    nlShutdown();
    initialized = false;
  }
}

NLaddress getLocalAddress() {
  assert(initialized);
  NLaddress ret;
  NLsocket temp = nlOpen(0, NL_RELIABLE);
  nlGetLocalAddr(temp, &ret);
  nlClose(temp);
  return ret;
}

int getBytesRead() {
  assert(initialized);
  return nlGetInteger(NL_BYTES_RECEIVED);
}

int getBytesWritten() {
  assert(initialized);
  return nlGetInteger(NL_BYTES_SENT);
}

void enableStats() {
  assert(initialized);
  nlEnable(NL_SOCKET_STATS);
}

void disableStats() {
  assert(initialized);
  nlDisable(NL_SOCKET_STATS);
}

void clearStats() {
  assert(initialized);
  nlClear(NL_ALL_STATS);
}

int getOpenConnections() {
  assert(initialized);
  return nlGetInteger(NL_OPEN_SOCKETS);
}

void setUserVersion(int version) {
  assert(initialized);
  userVersion = version;
}

}


