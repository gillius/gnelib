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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/Address.h"

std::ostream& operator << (std::ostream& o, const GNE::Address& addr) {
  return o << addr.toString();
}

namespace GNE {

//##ModelId=3BB974B402E4
Address::Address(std::string address) {
  setAddressByName(address);
}

//##ModelId=3BB94B190168
Address::Address(const char* address) {
  setAddressByName(address);
}

Address::Address(NLaddress address) : addr(address) {
}

//##ModelId=3BB9282B0334
Address::~Address() {
}

//##ModelId=3BB974B40384
void Address::setAddressByName(std::string address) {
  nlGetAddrFromName((NLbyte*)address.c_str(), &addr);
}

//##ModelId=3BBA41220208
std::string Address::getNameByAddress() const {
  char buf[NL_MAX_STRING_LENGTH];
  nlGetNameFromAddr(&const_cast<Address*>(this)->addr, (NLbyte*)buf);
  return std::string(buf);
}

//##ModelId=3BB9282B0336
NLaddress Address::getAddress() const {
  return addr;
}

//##ModelId=3BB9282B0371
void Address::setAddress(NLaddress address) {
  addr = address;
}

//The totally lame const_casts are due to HawkNL's lack of const-correctness.
//##ModelId=3BB92893038E
std::string Address::toString() const {
  if (isValid()) {
    char buf[NL_MAX_STRING_LENGTH];
    nlAddrToString(&const_cast<Address*>(this)->addr, (NLbyte*)buf);
    return std::string(buf);
  } else
    return "invalid address";
}

//##ModelId=3BB94B1901D6
int Address::getPort() const {
  return (int)nlGetPortFromAddr(&const_cast<Address*>(this)->addr);
}

//##ModelId=3BB932340050
void Address::setPort(int port) {
  if (port < 0 || port > 65535)
    addr.valid = NL_FALSE;
  else
    nlSetAddrPort(&const_cast<Address*>(this)->addr, (NLushort)port);
}

//##ModelId=3BB934B4015E
bool Address::isValid() const {
  return (addr.valid == NL_TRUE);
}

//##ModelId=3BB934C5010E
Address::operator bool() const {
  return isValid();
}

//##ModelId=3BB934CC00BE
bool Address::operator ==(const Address& rhs) const {
  return (nlAddrCompare(&const_cast<Address*>(this)->addr, &const_cast<Address&>(rhs).addr) == NL_TRUE);
}

} //namespace GNE




