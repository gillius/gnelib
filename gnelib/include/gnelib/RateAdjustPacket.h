#ifndef _RATEADJUSTPACKET_H_
#define _RATEADJUSTPACKET_H_

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

#include "Packet.h"

namespace GNE {

/**
 * The RateAdjustPacket is sent automatically by GNE when you change rates in
 * the PacketStream.  It is not a packet that you send, or that you will
 * see -- it is used only internally by GNE.
 */
//##ModelId=3C783ACF00AE
class RateAdjustPacket : public Packet {
public:
  //##ModelId=3C783ACF02AA
  RateAdjustPacket();

  //##ModelId=3C783ACF02AB
  virtual ~RateAdjustPacket();

  /**
   * The ID for this type of packet.
   */
  //##ModelId=3C783ACF02A1
  static const int ID;

  /**
   * Returns a newly allocated exact copy of this packet.
   */
  //##ModelId=3C783ACF02AD
  virtual Packet* makeClone() const;

  /**
   * Returns the current size of this packet in bytes.
   */
  //##ModelId=3C783ACF02AF
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket. 
   */
  //##ModelId=3C783ACF02B1
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.
   */
  //##ModelId=3C783ACF02B4
  virtual void readPacket(RawPacket& raw);

  /**
   * Returns a new instance of this class using the default constructor.
   */
  //##ModelId=3C783ACF02B7
  static Packet* create();

  /**
   * The requested rate.
   */
  //##ModelId=3C783ADD03D8
  guint32 rate;

private:
};

}
#endif
