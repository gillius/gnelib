#ifndef _EXITPACKET_H_
#define _EXITPACKET_H_

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
#include "Packet.h"

namespace GNE {

/**
 * The ExitPacket is sent automatically by GNE when you disconnect in a
 * proper manner.  You need not use this class.  When you receive an
 * ExitPacket, though, a graceful disconnect is initiated that generates an
 * onExit event.
 */
class ExitPacket : public Packet {
public:
  //##ModelId=3C7071E00170
  ExitPacket();

  //##ModelId=3C7071E00171
  virtual ~ExitPacket();

  /**
   * The ID for this type of packet.
   */
  //##ModelId=3C7071E0016F
  static const int ID;

  /**
   * Returns a newly allocated exact copy of this packet.
   */
  //##ModelId=3C7071E00173
  virtual Packet* makeClone() const;

  /**
   * Returns the current size of this packet in bytes.
   */
  //##ModelId=3C7071E00178
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket. 
   */
  //##ModelId=3C7071E0017A
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.
   */
  //##ModelId=3C7071E0017D
  virtual void readPacket(RawPacket& raw);

  /**
   * Returns a new instance of this class using the default constructor.
   */
  //##ModelId=3C7071E00180
  static Packet* create();

private:
};

}
#endif
