#ifndef PACKET_H_INCLUDED_C51B374A
#define PACKET_H_INCLUDED_C51B374A

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
class RawPacket;

/**
 * The base packet class, used for dealing with all types of packets at a
 * fundamental level.  All types of packets must inherit from this class to
 * be recognized by GNE.  GNE already implmenets some types of packets.
 */
//##ModelId=3AE450C3028A
class Packet {
public:
  //##ModelId=3AE4D49C0168
  Packet();

  //##ModelId=3AE4D49C019A
  virtual ~Packet();

  /**
   * Creates and allocates a new RawPacket containing the serialized form of
   * the packet.
   * @param reliable should this packet be sent reliably?
   */
  //##ModelId=3AE4D51000FA
  RawPacket* createRaw(bool reliable);

  /**
   * Returns the type of this packet.
   */
  //##ModelId=3AE4DA920276
  NLubyte getType();

  /**
   * Creates a newly allocated Packet from a RawPacket.
   * @param packet the RawPacket to parse from
   */
  //##ModelId=3AE4D58E0136
  static Packet* parsePacket(RawPacket& packet);

private:
  //##ModelId=3AE4D15E021C
  NLubyte type;

};



#endif /* PACKET_H_INCLUDED_C51B374A */


