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
 * exaddr
 * Small test/example using the Address class.
 */

#include "../../include/gnelib.h"
#include <iostream>

using namespace std;
using namespace GNE;
using namespace GNE::Console;

void displayAddress(const Address& addr);

int main(int argc, char* argv[]) {
  if (initGNE(NL_IP, atexit)) {
		exit(1);
	}
  initConsole(atexit);
  setTitle("GNE Address Class Test/Example");

	string url = "www.microsoft.com:80";

	//Explicit url resolving
	cout << url << " resolves to: " << Address(url).toString() << endl;
	//Test implicit conversion from string to Address
	cout << "localhost:1234 resolves to: ";
	displayAddress("localhost:1234");
	cout << endl;

	//Test other functions
	Address addr("145.64.129.123");
	cout << "145.64.129.123 resolves to: " << addr << endl;
	cout << "Port is: " << addr.getPort() << endl;
	cout << "Setting port to 15:";
	addr.setPort(15);
	cout << " address is now: " << addr << endl;

	cout << "Trying to set port to -516 (invalid)";
	addr.setPort(-516);
	if (addr)
		cout << " address still valid (NOT CORRECT)." << endl;
	else
		cout << " address no longer valid (this is correct)." << endl;

	Address rTest("www.yahoo.com:80");
	cout << "Testing reverse DNS: www.yahoo.com:80 resolves to " << rTest << endl;
	cout << rTest << " reverses to " << rTest.getNameByAddress() << endl;

	cout << "Try picking your own address: ";
	cin >> url;
	cout << url << " resolves to: " << Address(url) << endl;
	cout << Address(url) << " reverses to " << Address(url).getNameByAddress() << endl;

	return 0;
}

void displayAddress(const Address& addr) {
	cout << addr;
}
