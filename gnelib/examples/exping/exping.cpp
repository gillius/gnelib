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
 * exping -- Shows how to use the PingPacket API and how to create and
 * respond to PingPackets.  Also functions as a test of the PingPacket
 * class.
 */

#include "../../include/gnelib.h"
#include <iostream>

using namespace std;
using namespace GNE;
using namespace GNE::PacketParser;
using namespace GNE::Console;

void errorExit(const char* error);
int getPort(const char* prompt);
void doServer(int outRate, int inRate, int port);
void doClient(int outRate, int inRate, int port);
void doLocalTest();

//The PingTest class works on both the client and the server side.
class PingTest : public ConnectionListener {
public:
  PingTest() : conn(NULL) {}
  ~PingTest() {}

  void onDisconnect() {
    //Get any packets that are left and shut down.
    receivePackets();
    delete conn;
    delete this;
  }

  void onConnect(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();
  }

  void onNewConn(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();
  }

  void onReceive() {
    receivePackets();
  }

  void onFailure(const Error& error) {
    gout << "Socket failure: " << error << endl;
    //No need to disconnect, this has already happened on a failure.
  }

  void onError(const Error& error) {
    gout << "Socket error: " << error << endl;
    conn->disconnect();//For simplicity we treat even normal errors as fatal.
  }

  void onConnectFailure(const Error& error) {
    gout << "Connection to server failed.   " << endl;
    gout << "  GNE reported error: " << error << endl;
  }

  void receivePackets() {
    //We check for PingPackets.  Anything else we get is an error though.
    Packet* next = conn->stream().getNextPacket();
    while (next != NULL) {
      if (next->getType() == PingPacket::ID) {
        PingPacket* ping = (PingPacket*)next;
        if (ping->isRequest()) {
          ping->makeReply();
          conn->stream().writePacket(*ping, true);
        } else {
          gout << "Ping response: " << ((PingPacket*)next)->getPing() << endl;
        }
      } else {
        gout << "We got a packet type (" << next->getType() <<
          ") that we aren't expecting!" << endl;
      }
      delete next;
      next = conn->stream().getNextPacket();
    }
  }

private:
  Connection* conn;
};

class OurListener : public ServerConnectionListener {
public:
  OurListener() 
    : ServerConnectionListener() {
  }

  virtual ~OurListener() {}

  void onListenFailure(const Error& error, const Address& from, ConnectionListener* listener) {
    gout << "Connection error: " << error;
    gout << "  Error received from " << from;
    delete listener;
  }

  void getNewConnectionParams(int& inRate, int& outRate,
                              bool& allowUnreliable,
                              ConnectionListener*& listener) {
    inRate = outRate = 0; //0 meaning no limits on rates.
    //We don't use unreliable transfers in this program.
    allowUnreliable = true;
    listener = new PingTest();
  }

private:
};

void errorExit(const char* error) {
  gout << "Error: " << error << endl;
  exit(1);
}

int getPort(const char* prompt) {
  int port;
  gout << "Which port should we " << prompt << ": ";
  gin >> port;
  while ((port < 1) || (port > 65535)) {
    gout << "Ports range from 1 to 65535, please select one in this range: ";
    gin >> port;
  }
  return port;
}

int main() {
  if (initGNE(NL_IP, atexit)) {
    cout << "Unable to initialize GNE" << endl;
    exit(2);
  }

  setGameInformation("GNE exping", 1);

  if (initConsole(atexit)) {
    cout << "Unable to initialize GNE Console" << endl;
    exit(3);
  }
  setTitle("GNE PingPacket Example");

  gout << "GNE PingPacket Example for " << GNE::VER_STR << endl;
  gout << "Local address: " << getLocalAddress() << endl;
  gout << "Should we act as the server, or the client?" << endl;
  gout << "Type 1 for client, 2 for server, or 3 for a local test: ";
  int type;
  gin >> type;

  int port;
  if (type == 2) {
    setTitle("GNE Net Performance Tester -- Server");
    gout << "Reminder: ports <= 1024 on UNIX can only be used by the superuser." << endl;
    port = getPort("listen on");
    doServer(0, 0, port);
  } else if (type == 1) {
    setTitle("GNE Net Performance Tester -- Client");
    port = getPort("connect to");
    doClient(0, 0, port);
  } else {
    doLocalTest();
  }

  return 0;
}

void doServer(int outRate, int inRate, int port) {
#ifdef _DEBUG
  //Generate debugging logs to server.log if in debug mode.
  initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL5, "server.log");
#endif
  OurListener server;
  if (server.open(port))
    errorExit("Cannot open server socket.");
  if (server.listen())
    errorExit("Cannot listen on server socket.");

  gout << "Server is listening on: " << server.getLocalAddress() << endl;
  gout << "Press a key to shutdown server." << endl;
  getch();
  //When the server class is destructed, it will stop listening and shutdown.
}

void doClient(int outRate, int inRate, int port) {
#ifdef _DEBUG
  initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL5, "client.log");
#endif
  string host;
  gout << "Enter hostname or IP address: ";
  gin >> host;

  Address address(host);
  address.setPort(port);
  if (!address)
    errorExit("Invalid address.");
  gout << "Connecting to: " << address << endl;
  gout << "Press a key to stop the testing. " << endl;

  //We allocate it on the heap because PingTest deletes this connection.
  ClientConnection* client = new ClientConnection(new PingTest());
  if (client->open(address, outRate, inRate, 0, false))
    errorExit("Cannot open client socket.");

  client->connect();
  client->join();

  if (client->isConnected()) {
    gout << "Press s to send a ping, u to send a ping over unreliable, and q to quit." << endl;
    int ch = 0;
    while (ch != (int)'q') {
      ch = getch();
      if (ch == (int)'s') {
        PingPacket req;
        client->stream().writePacket(req, true);
      } else if (ch == (int)'u') {
        PingPacket req;
        client->stream().writePacket(req, false);
      }
    }
  } else {
    gout << "An error occured while connecting.  Press a key." << endl;
    getch();
  }

  client->disconnectSendAll();
}

void doLocalTest() {
  //Just creating the PingPacket registers the request and measures the time
  //from this point until we get it back.
  gout << "Creating a PingPacket." << endl;
  PingPacket test;
  assert(PingPacket::reqsPending() == 1);

  //Then the other side normally would receive a packet.  No reason why we
  //can't reply to ourselves!
  test.makeReply();

  //Then when you get the reply back, you call getPing on it.  This looks up
  //the unique request ID and finds the time difference, THEN THE ID IS
  //REMOVED FROM THE TABLE.  This means getPing will only work once for each
  //ping!  If you try to call it again or on an invalid reply, it will return
  //a 0 ping time.
  gout << "The ping time is: " << test.getPing()
    << " seconds. (should be VERY small)" << endl;
  assert(PingPacket::reqsPending() == 0);
  gout << "An invalid ping packet ping time (should be 0): "
    << test.getPing() << endl;

  gout << "Creating 3 PingPackets which we will let be 'late'" << endl;
  PingPacket l1, l2, l3;
  l1.makeReply(); //Make l1 a reply... We will try to use it later...
  assert(PingPacket::reqsPending() == 3);

  gout << "Creating another PingPacket." << endl;
  PingPacket longTest;
  assert(PingPacket::reqsPending() == 4);
  gout << "Waiting 350 ms." << endl;
  Thread::sleep(350);
  longTest.makeReply(); //It really doesn't matter when we call makeReply.
  gout << "Ping time: " << longTest.getPing() << endl;
  assert(PingPacket::reqsPending() == 3);

  gout << "Waiting another 200ms." << endl;
  Thread::sleep(200);
  gout << "Declaring requests older than 500ms as 'late'.  Found: "
    << PingPacket::recoverLostRequests(Time(0, 500000)) << '.' << endl;
  assert(PingPacket::reqsPending() == 0);
  gout << "Ping for one of those late, removed, requests (should be 0): "
    << l1.getPing() << endl;

  gout << "Press a key to continue." << endl;
  getch();
}
