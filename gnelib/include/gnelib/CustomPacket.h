#ifndef _CUSTOMPACKET_H_
#define _CUSTOMPACKET_H_

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
class RawPacket;

/**
 * Many times, espically during connection, you want to send some more
 * "free-form" data that may or may not all be related and will only be sent
 * once.  It may not make sense to create a completely new packet type just
 * to send a few things one time.  This packet type will allow you to send
 * whatever you want.  Basically, CustomPacket is just a packet that contains
 * a RawPacket.  You can use the RawPacket to put in data and pull it out.
 * Remember RawPacket does endian and processor-type conversions for you.\n
 * See the documentation for Packet for more info on some of these functions.
 */
//##ModelId=3C0B257E02C7
class CustomPacket : public Packet {
public:
  //##ModelId=3C0B257E03C4
  CustomPacket();

  //##ModelId=3C0B257E03C5
  virtual ~CustomPacket();

  /**
   * The ID for this type of packet.
   */
  //##ModelId=3C65C6D00076
  static const int ID;

  /**
   * Returns the RawPacket for reading or writing.  If a RawPacket already
   * exists through a call to getData or by a readPacket call, this method
   * will create a new RawPacket ready for writing.\n
   * This RawPacket will last until this object is destroyed or readPacket
   * is called.
   */
  //##ModelId=3C0B257E03C7
  RawPacket& getData();

  /**
   * This method resets the underlying data, and prepares it for write
   * access.  Essentially, this method "recreates" the CustomPacket instance.
   */
  //##ModelId=3C7E97B80267
  void reset();

  /**
   * Returns a newly allocated exact copy of this packet.  Due to the nature
   * of RawPacket not knowing its size on reading (this should be inferred by
   * the packet types in it), this only copies the data up to the current
   * position.  So makeClone works perfectly on packets that are being used
   * for writing, but only clones the data read so far on packets that are
   * being used for reading.\n
   * Since makeClone is used by GNE when sending packets, the RawPackets were
   * just used for writing this function works acceptably.
   */
  //##ModelId=3C0B257E03C8
  virtual Packet* makeClone() const;

  /**
   * @see Packet::getSize()
   */
  //##ModelId=3C0B257E03CA
  virtual int getSize() const;

  /**
   * Returns the maximum possible size of the user's data in this packet. Use
   * this value rather than RawPacket::RAW_PACKET_LEN.  The value returned is
   * the maximum size the hosted RawPacket can be.
   */
  //##ModelId=3C30EFBD0226
  static int getMaxUserDataSize();

  /**
   * Writes the packet to the given RawPacket.  The RawPacket data should
   * only have been used for writing since its creation or its last reset,
   * since the length of the RawPacket is needed, and this is only known to
   * RawPackets used for writing only.
   */
  //##ModelId=3C0B257E03CC
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.  The data in this packet
   * will be destroyed and replaced by the new data.
   */
  //##ModelId=3C0B257E03CF
  virtual void readPacket(RawPacket& raw);

  /**
   * Returns a new instance of this class.
   */
  //##ModelId=3C0B257E03D2
  static Packet* create();

private:
  /**
   * Deletes the RawPacket and our buffer, if needed.
   */
  //##ModelId=3C0B257E03D4
  void destroyData();

  //##ModelId=3C0B25A3037C
  RawPacket* data;

  /**
   * If this is non-null, then the current RawPacket is using a buffer that
   * we allocated, and we need to delete it.
   */
  //##ModelId=3C65C7A201EB
  gbyte* ourBuf;

  /**
   * We have to be copyable to work with SyncConnection.  When we are read in
   * we know our contentLength at least until getData is called.  When this
   * variable is -1 we do not know the size of the CustomPacket.
   */
  //##ModelId=3C7E6B2203AF
  int contentLen;
};

} //namespace GNE

#endif //#ifndef _CUSTOMPACKET_H_
