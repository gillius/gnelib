#ifndef _SHARED_H_
#define _SHARED_H_

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

//This header contains the shared code between exhello and exsynchello.

#include "../../include/gnelib.h"

void doMain(const char* connType);
void errorExit(const char* error);
int getPort(const char* prompt);
void doServer(int outRate, int inRate, int port);
void doClient(int outRate, int inRate, int port);

class OurServer;

class HelloPacket : public Packet {
public:
  HelloPacket() : Packet(MIN_USER_ID) {}
  HelloPacket(string message2) : Packet(MIN_USER_ID), message(message2) {}
  virtual ~HelloPacket() {}

  Packet* makeClone() const {
    return new HelloPacket(*this);
  }

  int getSize() const {
    return Packet::getSize() + message.size() + 1;
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

class OurListener : public ServerConnectionListener {
public:
  OurListener() 
    : ServerConnectionListener() {
  }

  virtual ~OurListener() {}

  void onListenFailure(const Error& error, const Address& from, ConnectionListener* listener) {
    mprintf("Connection error: %s\n", error.toString().c_str());
    mprintf("  Error received from %s", from.toString().c_str());
    delete listener;
  }

  void getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener);

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

void doMain(const char* connType) {
  if (initGNE(NL_IP, atexit)) {
    exit(1);
  }
  setUserVersion(1); //sets our user protocol version number, used in
                     //the connection process by GNE to version check.
  initConsole(atexit);
  setTitle("GNE Basic Connections Example");
  registerPacket(MIN_USER_ID, HelloPacket::create);

  gout << "GNE " << connType << " Basic Connections Example for " << GNE::VER_STR << endl;
  gout << "Local address: " << getLocalAddress() << endl;
  gout << "Should we act as the server, or the client?" << endl;
  gout << "Type 1 for client, 2 for server: ";
  int type;
  gin >> type;

  int port;
  if (type == 2) {
    setTitle("GNE Basic Connections Example -- Server");
    gout << "Reminder: ports <= 1024 on UNIX can only be used by the superuser." << endl;
    port = getPort("listen on");
    doServer(3000, 3000, port);
  } else {
    setTitle("GNE Basic Connections Example -- Client");
    port = getPort("connect to");
    doClient(3000, 3000, port);

    gout << "Press a key to continue." << endl;
    getch();
  }
}

void doServer(int outRate, int inRate, int port) {
#ifdef _DEBUG
  //Generate debugging logs to server.log if in debug mode.
  initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "server.log");
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

#endif
