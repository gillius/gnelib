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
 * exrawtest -- Basically a test of the RawPacket class.  Does show usage of the
 * RawPacket class, but the usage in this case because this is a test uses the
 * class is "hackish" ways.  The standard use of RawPackets is pretty trivial.
 */

#include "../../include/gnelib.h"
#include <iostream>

using namespace std;
using namespace GNE;
using namespace GNE::Console;

int main(int argc, char* argv[]) {
  initGNE(NO_NET, atexit);
  initConsole(atexit);
  Console::setTitle("GNE RawPacket Test");

  gout << " * Now doing endian tests." << endl;
  //Note: This code has nothing to do with the proper usage of RawPacket.
  //It is meant ONLY as a test to make sure RawPacket is properly converting
  //the the right endian format, and to discover this system's endian format.
  int x = 15;
  gout << "This system is little-endian: " << (((char*)&x)[0] == 15) << endl;
  gout << "This system is big-endian: " << (((char*)&x)[3] == 15) << endl;
  //Now test to make sure RawPacket converts from the system to little.
  RawPacket endianTest;
  endianTest << x;
  endianTest.reset();
  gbyte t[4];
  //Read the integer we just wrote into a raw byte array.
  endianTest.readRaw(t, 4);
  gout << "RawPacket must be in little-endian format." << endl;
  gout << "RawPacket is in little-endian format: " << (t[0] == 15) << endl;
  gout << "RawPacket is in big-endian format: " << (t[3] == 15) << endl;
  gout << endl;

  gout << " * Now writing a RawPacket..." << endl;

  //Create some binary and string data.
  gbyte block[16] = {'a', 'b', 'a', 'a', 'c', 'a', 'd', 'a', 'e', '1', 'a',
    'a', 'a', '4', 'a', 'a'};
  char nulls[10] = {'n', 'u', 'l', 'l', '\0', '\0', 't', 'e', 's', 't'};
  string nullsStr(nulls, 10);

  RawPacket raw(NULL); //same as RawPacket raw().
  raw << (gint32)56 << "superlala" << (guint8)124 << (gint16)26
      << (gsingle)12.5 << (gdouble)123.4 << nullsStr;
  raw.writeRaw(block, 16);
  
  gout << "Raw length: " << raw.getPosition() << endl;
  gout << " should be: " << (sizeof(int) + 10 + sizeof(NLubyte) +
                             sizeof(short) + sizeof(float) +
                             sizeof(double) + nullsStr.size() + 1 + 16) << endl;

  gbyte* buffer = new gbyte[raw.getPosition()];
  memcpy(buffer, raw.getData(), raw.getPosition());
  RawPacket raw2(buffer);
  gout << "Now reading from the same data..." << endl;

  //Create variables to read into that match those above.
  gint32 a;
  string b;
  guint8 c;
  gint16 d;
  gsingle e;
  gdouble f;
  string g;
  memset(block, 0, 16);

  //Read back.
  raw2 >> a >> b >> c >> d >> e >> f >> g;
  raw2.readRaw(block, 16);

  gout << a << endl << b << endl << (int)c << endl << d << endl
       << e << endl << f << endl;

  for (int count=0; count<16; count++) {
    gout << block[count] << ' ';
  }
  gout << endl;
  gout << "Data was of length: " << raw2.getPosition() << " (should be as above)" << endl;
  gout << "String1 length: " << b.length() << " (should be 9)" << endl;
  gout << "String2 length: " << g.length() << " (should be " << nullsStr.length() << ")" << endl;
  
  gout << "Press a key to continue: " << endl;
  getch();

  return 0;
}


