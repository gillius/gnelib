/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@webzone.net)
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

#include "gneintern.h"
#include "GNE.h"

//##ModelId=3AE26F6A035C
const double GNE::VER = 0.1;

//##ModelId=3AE26FC000A0
const std::string GNE::VER_STR = "GNE v0.1 pre-alpha";

//##ModelId=3AE270BF0078
bool GNE::init(NLenum networkType) {
  return true;
}

//##ModelId=3AE270CE033E
void GNE::shutdown() {
}

//##ModelId=3AEDEDE100AA
void GNE::exit(int status) {
  pthread_exit(&status);
}

//##ModelId=3AE4DF7A03AC
const int GNE::UDP_HEADER_SIZE = 28;

//##ModelId=3AE4C97C0244
int GNE::getBytesRead() {
  return nlGetInteger(NL_BYTES_RECEIVED);
}

//##ModelId=3AE4C988000A
int GNE::getBytesWritten() {
  return nlGetInteger(NL_BYTES_SENT);
}

//##ModelId=3AE4CA1D0122
void GNE::enableStats() {
}

//##ModelId=3AE4CA2803AC
void GNE::disableStats() {
}

//##ModelId=3AE4D10803CA
int GNE::getOpenConnections() {
  return nlGetInteger(NL_OPEN_SOCKETS);
}


