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
 * exhello
 * Everyone knows the "Hello World" program.  Perhaps it's been coded
 * too many times.  However, the GNE coders just plain don't care ;).
 * This is an example to show the minimum code needed to connect and send
 * data, in this case, "Hello Server (or Client)!" to the other end of the
 * connection.  This program uses asyncronous event-driven methods.
 * Well... perhaps not the minimum.  Lots of errors can happen over network
 * connections that we have to handle for this program to properly execute
 * when conditions aren't perfectly ideal, like when we can't reach the
 * server.
 */

#include "../../include/gnelib.h"
#include <iostream>
#include <string>

using namespace std;
using namespace GNE;
using namespace GNE::Console;
using namespace GNE::PacketParser;

#include "shared.h"

class OurClient : public ConnectionListener {
public:
  OurClient() : conn(NULL) {
    mprintf("Client listener created.\n");
  }

  ~OurClient() {
    mprintf("Client listener destroyed.\n");
  }

  void onDisconnect() { 
    mprintf("Client just disconnected.\n");
    //This is an iffy practice.  If we do this, we have to be careful to
    //always note that each new connection we use this listener with gets
    //its own new copy which we don't destroy later.
    delete this;
  }

  void onConnect(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();
    mprintf("Connection to server successful.\n");
  }

  void onReceive() {
    Packet* message = conn->stream().getNextPacket();
    if (message->getType() == MIN_USER_ID) {
      HelloPacket* helloMessage = (HelloPacket*)message;
      mprintf("got message: \"");
      mprintf(helloMessage->getMessage().c_str());
      mprintf("\"\n");
    } else
      mprintf("got bad packet.\n");
    delete message;
  }

  void onFailure(const Error& error) {
    mprintf("Socket failure: %s\n", error.toString().c_str());
    //No need to disconnect, this has already happened on a failure.
  }

  void onError(const Error& error) {
    mprintf("Socket error: %s\n", error.toString().c_str());
    conn->disconnect();//For simplicity we treat even normal errors as fatal.
  }

  void onConnectFailure(const Error& error) {
    mprintf("Connection to server failed.\n");
    mprintf("GNE reported error: %s\n", error.toString().c_str());
  }
private:
  Connection* conn;
};

class OurServer : public ConnectionListener {
public:
  OurServer() : conn(NULL), received(false) {
    mprintf("Server listener created\n");
  }

  virtual ~OurServer() {
    mprintf("Server listener killed\n");
  }

  void onDisconnect() { 
    mprintf("ServerConnection just disconnected.\n");
    if (!received)
      mprintf("No message received.\n");
    delete conn;
    delete this;
  }

  void onNewConn(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();
    mprintf("Connection received from %s; waiting for message...\n", conn->getRemoteAddress(true).toString().c_str());
  }

  void onReceive() {
    Packet* message = conn->stream().getNextPacket();
    if (message->getType() == MIN_USER_ID) {
      HelloPacket* helloMessage = (HelloPacket*)message;
      mprintf("got message: \"");
      mprintf(helloMessage->getMessage().c_str());
      mprintf("\"\n");
      received = true;

      //Send Response
      mprintf("  Sending Response...\n");
      HelloPacket response("Hello, client!  I'm the event-driven server!");
      conn->stream().writePacket(response, true);
    } else
      mprintf("got bad packet.\n");
    delete message;
  }

  void onFailure(const Error& error) {
    mprintf("Socket failure: %s\n", error.toString().c_str());
  }

  void onError(const Error& error) {
    mprintf("Socket error: %s\n", error.toString().c_str());
    conn->disconnect();
  }

private:
  Connection* conn;
  bool received;
};

void OurListener::getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener) {
  inRate = 3200;
  outRate = 3200;
  listener = new OurServer();
}

int main(int argc, char* argv[]) {
  doMain("Event-Driven");
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

  ClientConnection client(outRate, inRate, new OurClient());
  if (client.open(address, 0)) //localPort = 0, for any local port.
    errorExit("Cannot open client socket.");

  client.connect();
  client.join();     //join on the connection thread
  //if we did not call join, we would have called client.detach(false)
  //instead for true async connections.

  //Check if our connection was successful.
  if (client.isConnected()) {
    
    //Send our information
    HelloPacket message("Hello, server!  I'm the event-driven client!");
    client.stream().writePacket(message, true);
    client.stream().waitToSendAll();
    
  }
}


