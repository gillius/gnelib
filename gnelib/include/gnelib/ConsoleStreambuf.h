#ifndef _CONSOLESTREAMBUF_H_
#define _CONSOLESTREAMBUF_H_

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

#include "gneintern.h"
#include <streambuf>

namespace GNE {
namespace Console {

/**
 * A class derived from streambuf that uses the functions in the Console class.
 * This allows cin or another istream to work under GNE by using the Console
 * class for multithreaded I/O.\n
 * For specifics on these functions, consult a standard C++ library reference.
 */
//##ModelId=3BF844450360
class ginbuf : public std::streambuf {
public:
  //##ModelId=3BF8444503C9
  ginbuf();
  //##ModelId=3BF8444503CA
  ~ginbuf();

protected:
  //##ModelId=3BF8444503CB
  int_type underflow();

private:
  //##ModelId=3BF8444503C8
  char* buf;
};

/**
 * A class derived from streambuf that uses the functions in the Console class.
 * This allows cout or another ostream to work under GNE by using the Console
 * class for multithreaded I/O.\n
 * For specifics on these functions, consult a standard C++ library reference.
 */
//##ModelId=3BF844450364
class goutbuf : public std::streambuf {
public:
  //##ModelId=3BF8444503CD
  goutbuf();
  //##ModelId=3BF8444503CE
  ~goutbuf();

protected:
  //##ModelId=3BF8444503CF
  int sync();
  //##ModelId=3BF8444503D0
  void flush_output();
  //##ModelId=3BF8444503D1
  int_type overflow(int_type meta = traits_type::eof());
  //##ModelId=3BF8444503D3
  std::streamsize xsputn(const char_type *ptr, std::streamsize count);

private:
  //##ModelId=3BF8444503CC
  char* buf;
};

}
}

#endif //#ifndef _CONSOLESTREAMBUF_H_
