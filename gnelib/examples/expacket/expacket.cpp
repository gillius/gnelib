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

/**
 * expacket -- Shows how to derive a new type of packet, and create the
 * proper parsing functions interacting with RawPacket to allow GNE to use
 * your packet.
 * This example does NOT show how to use the packet over the network.
 * See exhello for that.
 */

#include "../../src/gnelib.h"
#include <iostream>

using namespace std;
using namespace GNE;
using namespace GNE::PacketParser;

#include "expacket.h"

int main(int argc, char* argv[]) {
  initGNE(NO_NET, atexit);
  Console::setTitle("GNE Packet Test");
  registerPacket(MIN_USER_ID, PersonPacket::create);
  //we don't register UnknownPacket to try to trigger an error later so we
  //can test error handling of the parser.
  
  cout << endl;
  packetTest();

  return 0;
}

void packetTest() {
  cout << "Creating two PersonPackets" << endl;
  PersonPacket jason;
  jason.age = 19;
  jason.firstName = "Jason";
  jason.lastName = "Winnebeck";
  jason.timestamp = 0;
    //We should always set the timestamp for a packet when it was created,
    //but since we aren't sending it over a network it doesn't really matter.
  PersonPacket elias;
  elias.age = 255;
  elias.firstName = "Elias";
  elias.lastName = "Pschernig";
  elias.timestamp = 0;

  RawPacket raw(NULL);
  jason.writePacket(raw);
  elias.writePacket(raw);

  raw.reset();

  PersonPacket t1;
  PersonPacket t2;

  /**
   * Read back the packets.  Note that we have to read in the ids, because
   * normally packets aren't used this way.  Normally the packet parser in GNE
   * will real the ID, then create the packet.  The packet needs not to read
   * its own ID.
   */
  NLubyte dummy;
  raw >> dummy;
  t1.readPacket(raw);
  raw >> dummy;
  t2.readPacket(raw);

  cout << t1.firstName << ' ' << t1.lastName << ", " << (int)t1.age << " years." << endl;
  cout << t2.firstName << ' ' << t2.lastName << ", " << (int)t2.age << " years." << endl;
}