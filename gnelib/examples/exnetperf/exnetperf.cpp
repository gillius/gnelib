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
 * exnetperf -- Shows how to use the statistical functions in GNE.  This
 * example should serve well for GNE to measure network performance, as best
 * as is possible with the GNE and HawkNL APIs.  This example uses event-
 * driven networking since that is what is most likely to be used once the
 * game has started, and the method that most likely has the largest impact
 * on performace.  (SyncConnection will most likely be used only while
 * connecting).
 * This example could also be used to test the bandwidth throttling.
 */

#include "../../include/gnelib.h"
#include <iostream>

using namespace std;
using namespace GNE;
using namespace GNE::PacketParser;
using namespace GNE::Console;

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

  void getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener) {
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
  setUserVersion(1); //sets our user protocol version number, used in
                     //the connection process by GNE to version check.
  if (initConsole(atexit)) {
    cout << "Unable to initialize GNE Console" << endl;
    exit(3);
  }
  setTitle("GNE Net Performance Tester");

  return 0;
}