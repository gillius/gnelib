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

/**
 * exbroker
 * Test of ObjectBrokerServer and ObjectBrokerClient and an example of how
 * to use these classes manually (outside of the rest of the high-level API.)
 */

#include "../../include/gnelib.h"
#include <string>
#include <sstream>
#include <assert.h>

using namespace std;

using namespace GNE;
using namespace GNE::PacketParser;
using namespace GNE::Console;

#include "PositionedText.h"

bool genericHandler( const Packet& packet, ObjectBrokerClient& obc );
void registerObjects();
void registerPackets();

int main(int argc, char* argv[]) {
  if (initGNE(NL_IP, atexit)) {
    exit(1);
  }
  initConsole(atexit);
  setTitle("GNE ObjectBrokers Test/Example");
  registerPackets();

  PositionedText t( "Test", 5, 10 );

  ObjectBrokerServer obs;
  ObjectBrokerClient obc;

  ObjectCreationPacket::pointer p = obs.getCreationPacket( t );

  gout << "Giving creation packet BEFORE registering that object type." << endl;
  gout << "This should generate an error message:" << endl;
  if ( genericHandler( *p, obc ) )
    gout << "--Test Success." << endl;
  else
    gout << "***** TEST FAILED! *****" << endl;

  getch();

  return 0;
}

bool genericHandler( const Packet& packet, ObjectBrokerClient& obc ) {
  try {
    PositionedText& obj =
      reinterpret_cast<PositionedText&>( obc.usePacket( packet ) );

    if ( obj.isDead() )
      delete &obj;
    else if ( !obj.isDrawn() )
      obj.draw();

    return false;

  } catch ( const Error& e ) {
    gout << e.toString() << endl;
    return true;
  }
}

void registerObjects() {
  ObjectBrokerClient::registerObject( PositionedTextPacket::ID,
    PositionedText::createFromPacket );
}

void registerPackets() {
  PacketParser::registerPacket( (guint8)PositionedTextPacket::ID,
    PositionedTextPacket::create );
  PacketParser::registerPacket( (guint8)PositionedTextPositionUpdatePacket::ID,
    PositionedTextPositionUpdatePacket::create );
  PacketParser::registerPacket( (guint8)PositionedTextTextUpdatePacket::ID,
    PositionedTextTextUpdatePacket::create );
}
