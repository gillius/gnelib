#ifndef _CONNECTIONSTATS_H_
#define _CONNECTIONSTATS_H_

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


namespace GNE {

/**
 * A struct holding values returned by getStats.
 */
//##ModelId=3C0D571B02A8
struct ConnectionStats {
  /**
   * Total bytes sent.
   */
  //##ModelId=3C0D571B03C7
  int bytesSent;
  /**
   * Total bytes received.
   */
  //##ModelId=3C0D571B03C8
  int bytesRecv;
  /**
   * Total low-level packets sent (not GNE packets but packets after they
   * were converted and translated to the current network).
   */
  //##ModelId=3C0D571B03C9
  int packetsSent;
  /**
   * Total low-level packets received.
   */
  //##ModelId=3C0D571B03CA
  int packetsRecv;
  /**
   * Maximum average bytes per second sent encountered.
   */
  //##ModelId=3C0D571B03CB
  int maxAvgBytesSent;
  /**
   * Maximum average bytes per second received encountered.
   */
  //##ModelId=3C0D571B03CC
  int maxAvgBytesRecv;
  /**
   * Average bytes per second sent in the last 8 seconds.
   */
  //##ModelId=3C0D571B03CD
  int avgBytesSent;
  /**
   * Average bytes per second received in the last 8 seconds.
   */
  //##ModelId=3C0D571B03CE
  int avgBytesRecv;
  /**
   * The number of open sockets.  This value is 0-2 when getting stats for
   * a single Connection.  When getting global stats this is all open
   * sockets, wether user controlled or maintained privately by GNE.
   */
  //##ModelId=3C106F02037E
  int openSockets;
};

} //namespace GNE

#endif //#ifndef _CONNECTIONSTATS_H_
