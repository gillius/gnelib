#ifndef _PACKETPARSER_H_
#define _PACKETPARSER_H_

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

namespace GNE {
class Packet;
class RawPacket;

/**
 * A namespace containing functions handling the parsing of packets.  Also
 * contains important constants to be aware of for assigning your packets
 * IDs.
 */
namespace PacketParser {

/**
 * The first number suitable for the user to give IDs to their packets.  Any
 * numbers between MIN_USER_ID and MAX_USER_ID inclusive belong to
 * the user.  It is suggested that the user assign packet numbers by adding
 * to MIN_USER_ID.  (i.e. MIN_USER_ID + 5)
 */
const guint8 MIN_USER_ID = 16;

/**
 * @see MIN_USER_ID
 */
const guint8 MAX_USER_ID = 254;

/**
 * The network packet ends with this byte, meaning that no more GNE packets
 * exist in this network packet (end-of-data).
 */
const guint8 END_OF_PACKET = 255;

/**
 * Registers a new type of packet, so GNE can recognize it.  In order for GNE
 * to recognize, create, and parse your packets derived from the Packet
 * class, you should register it here, preferably right after initalizing
 * GNE.\n
 * You can only register packets from MIN_USER_ID to MAX_USER_ID,
 * inclusive.  You may not register a packet multiple times.  As in other
 * parts of GNE, there are asserts to make this does not happen, but when
 * using the optimized GNE these checks are not done.
 */
void registerPacket(guint8 id, Packet* (*createFunc)());

/**
 * Parses the next packet from the given RawPacket.  Returns NULL on error.
 * if endOfPackets was set to true, then the NULL means no packets were left
 * to parse.  If endOfPackets was set to false, then NULL was due to network
 * packet corruption or a packet that GNE cannot recognize.  In this case,
 * the passed RawPacket is in an invalid and undefined state and should be
 * discarded.\n
 * Calling parseNextPacket after endOfPackets was set to true produces
 * undefined results.
 * @return a newly allocated Packet of the type of the next packet in the
 *         RawPacket, else NULL on error or end of RawPacket.
 */
Packet* parseNextPacket(bool& endOfPackets, RawPacket& raw);

}
}

#endif


