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
    gout << acquire << "Server instance created" << endl << release;
  }

  virtual ~OurServer() {
    gout << acquire << "ServerConnection instance killed" << endl << release;
  }

  void onDisconnect() { 
    gout << acquire;
    gout << "ServerConnection just disconnected." << endl;
    if (!received)
      gout << "  No message received." << endl;
    gout << release;

    delete ourConn;
    delete this;
  }

  void onNewConn(SyncConnection& conn) {
    ourConn = conn.getConnection();
    gout << acquire << "Connection received from "
         << conn.getConnection()->getRemoteAddress(true)
         << "; waiting for messages..." << endl << release;

    try {
      //We loop this twice because we will be getting two requests, because
      //exhello sends two packets to test reliable and unreliable testing,
      //but since we refused unreliable connections in OurListener, we will
      //get it reliably and on the SyncConnection.
      for (int c=0; c<2; ++c) {
        HelloPacket message;
        conn >> message;
        received = true;
        gout << acquire << "Got message: \"" << message.getMessage() << "\""
          << endl << release;
        
        HelloPacket response("Hello, client!  I'm the syncronous server!");
        conn << response;
      }
    } catch (Error& e) {
      gout << acquire;
      gout << "An error occured during communications." << endl;
      gout << "  The error was: " << e << endl;
      gout << release;
      throw;
    }
  }

private:
  //This is used only to delete the connection when it is disconnected.
  Connection* ourConn;
  bool received;
};

void OurListener::getNewConnectionParams(int& inRate, int& outRate,
                                         bool& allowUnreliable,
                                         ConnectionListener*& listener) {
  inRate = iRate;
  outRate = oRate;
  //We are only using sync connections, so we don't need the unreliable
  //socket.  If anyone wants to send unreliable data to us, it will come
  //through the reliable channel instead, so this is OK even though
  //exhello will try to request an unreliable connection.
  allowUnreliable = false;
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

  //Uncomment the loop for a stress test.
  //for (int i=0; i<100; ++i) {
    //We use pointers and new here only to allow the possibility for the for
    //loop.  Without the loop there is no reason why these cannot be on the
    //stack (auto variables).
    ClientConnection* clientConn
      = new ClientConnection(ConnectionListener::getNullListener());
    SyncConnection& client = *(new SyncConnection(clientConn));
    try {
      //Since only one client can exist, acquire and release is not needed on
      //gout.
      gout << "Opening client socket." << endl;
      client.open(address, 0, 0, 0, false);
      gout << "Attempting to connect." << endl;
      client.connect();
      
      gout << "Connection completed, sending messages to server." << endl;
      HelloPacket message("Hello, server!  I'm the syncronous client!");
      client << message;
      client << message;
      
      gout << "Waiting for messages back from server." << endl;
      client >> message;
      gout << "Received \"" << message.getMessage() << "\" from the server." << endl;
      client >> message;
      gout << "Received \"" << message.getMessage() << "\" from the server." << endl;
      
      gout << "Disconnecting." << endl;
      client.disconnect();

    } catch (Error& e) {
      gout << "An error occured while trying to communicate." << endl;
      gout << "  The error was: " << e << endl;
    }

    delete &client;
    delete clientConn;
  //}
}


