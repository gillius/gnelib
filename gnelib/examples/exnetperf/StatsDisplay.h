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

/**
 * StatsDisplay is a helper class for exnetperf which waits for some time
 * then updates stats.  The waiting is done because the faster we update the
 * screen, the more time we take away from network performance.  The less we
 * wait the better the feedback to the user.
 */

#include "../../include/gnelib.h"

using namespace std;
using namespace GNE;
using namespace GNE::PacketParser;
using namespace GNE::Console;

//This keeps track of some screen positions when can write to.
//Essentially makes tiles out of the screen.
struct Pos {
  int x, y;
  Connection* conn;
  int* packetsIn;
  int* packetsOut;
};
static Pos gpos = {0, 8, NULL}; //global stats position
static Pos positions[] = {{0, 17, NULL}};

class StatsDisplay : public TimerCallback {
public:
  /**
   * Sets up a StatsDisplay that updates the screen every "rate" milliseconds.
   */
  StatsDisplay(int rate) {
    timer = new Timer(this, rate, false);
  }

  virtual ~StatsDisplay() {
    delete timer;
  }

  /**
   * Starts the timer and starts displaying stats.
   */
  void startDisplay() {
    timer->startTimer();
  }

  /**
   * Stops what startDisplay started.
   */
  void stopDisplay() {
    timer->stopTimer(false);
  }

  /**
   * Adds a connection to the display list, returning true if there is not
   * enough screen space left.  In this case, the connection was not added
   * and stats will not be displayed.
   * @param conn the connection to add.
   * @param packetsIn a pointer to a place to get high-level packets in
   * @param packetsOut similar to packetsIn
   */
  bool addConn(Connection* conn, int* packetsIn, int* packetsOut) {
    //finish and make thread safe.
    positions[0].conn = conn;
    positions[0].packetsIn = packetsIn;
    positions[0].packetsOut = packetsOut;
    return false;
  }

  /**
   * Deletes a connection from the display list.  If the connection is not in
   * the list, this function has no effect.
   */
  void delConn(Connection* conn) {
    //finish and make thread safe.
    positions[0].conn = NULL;
  }

  /**
   * The callback function that updates the screen.
   */
  void timerCallback() {
    if (positions[0].conn != NULL)
      updateStats(positions[0]);
    updateGlobalStats();
  }

  /**
   * Redraws the stats for one connection.
   */
  void updateStats(const Pos& ourPos) {
    Connection* conn = ourPos.conn;

    ConnectionStats all = conn->getStats(-1);
    ConnectionStats rel = conn->getStats(1);
    ConnectionStats unrel = conn->getStats(0);
    //Printfs are so much nicer in this case over iostreams, eh?
    mlprintf(ourPos.x, ourPos.y, "%-20s%10s%10s%10s",
      conn->getRemoteAddress(true).toString().c_str(), "tot =" , "rel +", "unrel");
    mlprintf(ourPos.x, ourPos.y+1, "%-20s%10d", "HiLvlPkts in", *ourPos.packetsIn);
    mlprintf(ourPos.x, ourPos.y+2, "%-20s%10d", "HiLvlPkts out", *ourPos.packetsOut);
    mlprintf(ourPos.x, ourPos.y+3, "%-20s%10d%10d%10d",
      "bytesin", all.bytesRecv, rel.bytesRecv, unrel.bytesRecv);
    mlprintf(ourPos.x, ourPos.y+4, "%-20s%10d%10d%10d",
      "bytesout", all.bytesSent, rel.bytesSent, unrel.bytesSent);
    mlprintf(ourPos.x, ourPos.y+5, "%-20s%10d%10d%10d",
      "inRate", all.avgBytesRecv, rel.avgBytesRecv, unrel.avgBytesRecv);
    mlprintf(ourPos.x, ourPos.y+6, "%-20s%10d%10d%10d",
      "outRate", all.avgBytesSent, rel.avgBytesSent, unrel.avgBytesSent);
    mlprintf(ourPos.x, ourPos.y+7, "%-20s%10d%10d%10d",
      "maxInRate", all.maxAvgBytesRecv, rel.maxAvgBytesRecv, unrel.maxAvgBytesRecv);
    mlprintf(ourPos.x, ourPos.y+8, "%-20s%10d%10d%10d",
      "maxOutRate", all.maxAvgBytesSent, rel.maxAvgBytesSent, unrel.maxAvgBytesSent);
    mlprintf(ourPos.x, ourPos.y+9, "%-20s%10d%10d%10d",
      "sockets", all.openSockets, rel.openSockets, unrel.openSockets);
  }

  /**
   * Redraws the global stats.
   */
  void updateGlobalStats() {
    ConnectionStats all = GNE::getGlobalStats();
    mlprintf(gpos.x, gpos.y, "%-20s%10s", "global", "tot");
    mlprintf(gpos.x, gpos.y+1, "%-20s%10d", "bytesin", all.bytesRecv);
    mlprintf(gpos.x, gpos.y+2, "%-20s%10d", "bytesout", all.bytesSent);
    mlprintf(gpos.x, gpos.y+3, "%-20s%10d", "inRate", all.avgBytesRecv);
    mlprintf(gpos.x, gpos.y+4, "%-20s%10d", "outRate", all.avgBytesSent);
    mlprintf(gpos.x, gpos.y+5, "%-20s%10d", "maxInRate", all.maxAvgBytesRecv);
    mlprintf(gpos.x, gpos.y+6, "%-20s%10d", "maxOutRate", all.maxAvgBytesSent);
  }

private:
  Timer* timer;

  Mutex sync;
};
