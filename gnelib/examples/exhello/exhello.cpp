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
 * connection.
 * Well... perhaps not the minimum.  Lots of errors can happen over network
 * connections that we have to handle for this program to properly execute
 * when conditions aren't perfectly ideal, like when we can't reach the
 * server.
 */

#include "../../src/gnelib.h"
#include <iostream>
#include <cstring>
#include <string>

using namespace std;
using namespace GNE;
using namespace GNE::Console;
using namespace GNE::PacketParser;

void errorExit(const char* error);
int getPort(const char* prompt);
void displayAddress();
string getAddressString(NLaddress addr);
void doServer(int outRate, int inRate, int port);
void doClient(int outRate, int inRate, int port);

class HelloPacket : public Packet {
public:
	HelloPacket() : Packet(MIN_USER_ID) {}
	HelloPacket(string message2) : Packet(MIN_USER_ID), message(message2) {}
  virtual ~HelloPacket() {}

  Packet* makeClone() const {
    return new HelloPacket(*this);
  }

  int getSize() const {
    return Packet::getSize() + sizeof(message) + 1;
  }

  void writePacket(RawPacket& raw) const {
    Packet::writePacket(raw);
    raw << message;
  }

  void readPacket(RawPacket& raw) {
    Packet::readPacket(raw);
    raw >> message;
  }

  static Packet* create() {
    return new HelloPacket();
  }

	string getMessage() {
		return message;
	}

private:
	string message;
};

class OurClient : public ClientConnection {
public:
  OurClient(int outRate, int inRate) 
    : ClientConnection(outRate, inRate) {
		mprintf("Client instance created.\n");
	}

	virtual ~OurClient() {
		mprintf("Client instance killed.\n");
	}

  void onConnect() {
    mprintf("Connection to server successful.\n");
  }

	void onFailure(Connection::FailureType errorType) {
		Connection::onFailure(errorType);
		mprintf("Socket failure: %s\n", Connection::FailureStrings[errorType].c_str());
	}

  void onConnectFailure(FailureType errorType) {
    mprintf("Connection to server failed.\n");
  }
private:
};

class OurServer : public ServerConnection {
public:
  OurServer(int outRate, int inRate, NLsocket rsocket2)
    : ServerConnection(outRate, inRate, rsocket2), received(false) {
		mprintf("Server instance created\n");
	}

  virtual ~OurServer() {
		mprintf("Server instance killed\n");
	}

  void onNewConn() {
    mprintf("Connection received; waiting for message...\n");
		quit.acquire();
		quit.timedWait(5000);
		quit.release();
		if (!received)
			mprintf("No message received.\n");
    detach(true);
  }

	void onReceive() {
		Packet* message = stream().getNextPacket();
		if (message->getType() == MIN_USER_ID) {
			HelloPacket* helloMessage = (HelloPacket*)message;
			mprintf("got message: \"");
			mprintf(helloMessage->getMessage().c_str());
			mprintf("\"\n");
			received = true;
		} else
			mprintf("got bad packet.\n");
		delete message;
	}

	void onFailure(Connection::FailureType errorType) {
		Connection::onFailure(errorType);
		mprintf("Socket failure: %s\n", Connection::FailureStrings[errorType].c_str());
	}

  void onConnFailure(FailureType errorType) {
    mprintf("Connection failure while connecting.\n");
    detach(true);
  }
private:
	ConditionVariable quit;
	bool received;
};

class OurCreator : public ServerConnectionCreator {
public:
  OurCreator() {}
  virtual ~OurCreator() {}

  ServerConnection* create(int outRate, int inRate, NLsocket rsocket2) {
    return new OurServer(outRate, inRate, rsocket2);
  }
};

class OurListener : public ServerConnectionListener {
public:
  OurListener(int outRate, int inRate) 
    : ServerConnectionListener(outRate, inRate, new OurCreator()) {
  }

  virtual ~OurListener() {}

private:
};

int main(int argc, char* argv[]) {
  if (initGNE(NL_IP, atexit)) {
		exit(1);
	}
  setUserVersion(1); //sets our user protocol version number, used in
                     //the connection process by GNE to version check.
  initConsole(atexit);
  setTitle("GNE Basic Connections Example");
  registerPacket(MIN_USER_ID, HelloPacket::create);

  //It's okay to use iostreams instead of the Console functions when we are
  //only accessing the console from one thread.
  displayAddress();
  cout << "Should we act as the server, or the client?" << endl;
  cout << "Type 1 for client, 2 for server: ";
  int type;
  cin >> type;

  int port;
  if (type == 2) {
    setTitle("GNE Basic Connections Example -- Server");
    cout << "Reminder: ports <= 1024 on UNIX can only be used by the superuser." << endl;
    port = getPort("listen on");
    doServer(3000, 3000, port);
  } else {
    setTitle("GNE Basic Connections Example -- Client");
    port = getPort("connect to");
    doClient(3000, 3000, port);
  }

  return 0;
}

void errorExit(const char* error) {
  cout << "Error: " << error << endl;
  exit(1);
}

int getPort(const char* prompt) {
  int port;
  cout << "Which port should we " << prompt << ": ";
  cin >> port;
  while ((port < 1) || (port > 65535)) {
    cout << "Ports range from 1 to 65535, please select one in this range: ";
    cin >> port;
  }
  return port;
}

void displayAddress() {
  NLaddress ourAddr = getLocalAddress();
  char buf[NL_MAX_STRING_LENGTH];
  nlAddrToString(&ourAddr, (NLbyte*)buf);
  char* temp = strchr(buf, ':');
  if (temp)
    *temp = '\0';
  cout << "Local address: " << buf << endl;
}

string getAddressString(NLaddress addr) {
  char buf[NL_MAX_STRING_LENGTH];
  nlAddrToString(&addr, (NLbyte*)buf);
  return string(buf);
}

void doServer(int outRate, int inRate, int port) {
#ifdef _DEBUG
	initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "server.log");
#endif
  OurListener server(outRate, inRate);
  if (server.open(port))
    errorExit("Cannot open server socket.");
  if (server.listen())
    errorExit("Cannot listen on server socket.");

  cout << "Server is listening on: " << getAddressString(server.getLocalAddress()) << endl;
  cout << "Press a key to shutdown server." << endl;
  getch();
  //When the server class is destructed, it will stop listening and shutdown.
}

void doClient(int outRate, int inRate, int port) {
#ifdef _DEBUG
	initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "client.log");
#endif
  string host;
  cout << "Enter hostname or IP address: ";
  cin >> host;

  NLaddress addr;
  nlGetAddrFromName((NLbyte*)host.c_str(), &addr);
  nlSetAddrPort(&addr, (NLushort)port);
  
  cout << "Connecting to: " << getAddressString(addr) << endl;
  OurClient client(outRate, inRate);
  if (client.open(addr)) //let port take 0 default, for any local port.
    errorExit("Cannot open client socket.");
  client.connect();
  client.join();     //join on the connection thread
	HelloPacket message("Hello, server!");
	client.stream().writePacket(message, true);
	client.stream().waitToSendAll();
  //if we did not call join, we would have called client.detach(false)
  //instead for true async connections.
}


