#ifndef RAWPACKET_H_INCLUDED_C51B6961
#define RAWPACKET_H_INCLUDED_C51B6961

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

/**
 * A utility class for creating a raw block of data that is used to directly
 * send data through the socket, and on the other side, parse it into
 * system-dependant data.  Users of GNE can also use this class to serialize
 * packets into a form suitable to be sent over their own types of network
 * connections or saved/loaded from a file.  A neat trick might be implemented
 * where a file can "mimic" a server to provide the necessary packets to
 * restore the game state.
 */
//##ModelId=3AE4D1C600F0
class RawPacket {
public:
  //##ModelId=3AE4D3B30320
  RawPacket(bool alloc);

  //##ModelId=3AE4D3BD008C
  virtual ~RawPacket();

  /**
   * Gets the raw block of data.
   */
  //##ModelId=3AE4D3F303C0
  NLbyte* getData();

  /**
   * Resets the reading/writing position back to 0.
   */
  //##ModelId=3AE4D67700F0
  void reset();

private:
  //##ModelId=3AE4D1E50226
  int length;

  //##ModelId=3AE4D1ED0014
  NLbyte* data;

  /**
   * The current location of the next data read/write
   */
  //##ModelId=3AE4D40A035C
  int currLoc;

};



#endif /* RAWPACKET_H_INCLUDED_C51B6961 */


