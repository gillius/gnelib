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
 * exsynchello
 * This is the same thing as exhello, but using only SyncConnections.
 */

#include "../../include/gnelib.h"
#include <iostream>
#include <string>

using namespace std;
using namespace GNE;
using namespace GNE::Console;
using namespace GNE::PacketParser;

#include "../exhello/shared.h"

class OurServer : public ConnectionListener {
public:
  OurServer() : received(false) {
		gout << "Server instance created" << endl;
	}

  virtual ~OurServer() {
		gout << "ServerConnection instance killed" << endl;
	}

	void onDisconnect() { 
		gout << "ServerConnection just disconnected." << endl;
		if (!received)
			gout << "No message received." << endl;
		delete this;
	}

  void onNewConn(SyncConnection& conn) {
    gout << "Connection received from " << conn.getConnection()->getRemoteAddress(true)
         << "; waiting for message..." << endl;

    HelloPacket message;
    try {
      conn >> message;
      received = true;
      gout << "Got message: \"" << message.getMessage() << "\"" << endl;
      
      HelloPacket response("Hello, client!  I'm the syncronous server!");
      conn << response;
    } catch (Error e) {
      gout << "An error occured during communications." << endl;
      gout << "  The error was: " << e << endl;
    }
  }

private:
	bool received;
};

void OurListener::getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener) {
		inRate = 3200;
    outRate = 3200;
    listener = new OurServer();
}

int main(int argc, char* argv[]) {
  doMain("Syncronous");
  return 0;
}

void doClient(int outRate, int inRate, int port) {
#ifdef _DEBUG
	initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "client.log");
#endif
  string host;
  gout << "Enter hostname or IP address: ";
  gin >> host;

	Address address(host);
	address.setPort(port);
	if (!address)
		errorExit("Invalid address.");
  gout << "Connecting to: " << address << endl;

  ClientConnection clientConn(outRate, inRate, ConnectionListener::getNullListener());
  SyncConnection client(&clientConn);
  try {
    client.open(address, 0); //localPort = 0, for any local port.
    client.connect();
    
    HelloPacket message("Hello, server!  I'm the syncronous client!");
    client << message;

    client >> message;
    gout << "Received \"" << message.getMessage() << "\" from the server." << endl;
    
    client.disconnect();
  } catch (Error e) {
    gout << "An error occured while trying to communicate." << endl;
    gout << "  The error was: " << e << endl;
  }
}


