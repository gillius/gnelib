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
static int positionsLength = 3;
static Pos positions[] = {
  {0, 13, NULL}, {0, 20, NULL}, {0, 27, NULL}, {0, 34, NULL}
};

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
    //Since we have multiple exit points, we use this convience class.
    LockMutex lock(sync);

    Pos* pos = findPos(NULL);
    if (pos != NULL) {
      pos->conn = conn;
      pos->packetsIn = packetsIn;
      pos->packetsOut = packetsOut;
      return false;
    }

    return true;
  }

  /**
   * Deletes a connection from the display list.  If the connection is not in
   * the list, this function has no effect.
   */
  void delConn(Connection* conn) {
    sync.acquire();
    Pos* pos = findPos(conn);
    if (pos != NULL) {
      pos->conn = NULL;
      pos->packetsIn = pos->packetsOut = NULL;

      //Notify the user that the current screen location has finished its job
      //(for now at least) by printing disconnected over the IP addr.
      //We don't delete all of the data in case the user wants to trap the
      //text to keep the information.
      mlprintf(pos->x, pos->y, "* disconnected *  ");
    }
    sync.release();
  }

  /**
   * The callback function that updates the screen.
   */
  void timerCallback() {
    sync.acquire();
    for (int c = 0; c < positionsLength; ++c)
      if (positions[c].conn != NULL)
        updateStats(positions[c]);
    updateGlobalStats();
    sync.release();
  }

  /**
   * Redraws the stats for one connection.
   */
  void updateStats(const Pos& ourPos) {
    Connection* conn = ourPos.conn;

    ConnectionStats all = conn->getStats(-1);
    ConnectionStats rel = conn->getStats(1);
    ConnectionStats unrel = conn->getStats(0);
    //C-style I/O so much nicer in this case over iostreams, eh?
    //Not that the C-style isn't that nice.  HTML would be our best choice
    //for formatting this data but we aren't in a web browser, are we ;) ?
    mlprintf(ourPos.x, ourPos.y, "%-18s%10s%10s%10s%11s%10s%10s",
      conn->getRemoteAddress(true).toString().c_str(), "in:tot =" , "rel +", "unrel",
      "out:tot =", "rel +", "unrel");
    mlprintf(ourPos.x, ourPos.y+1, "%-18s%10d%31d",
      "HiLvlPkts", *ourPos.packetsIn, *ourPos.packetsOut);
    mlprintf(ourPos.x, ourPos.y+2, "%-18s%10d%10d%10d",
      "sockets", all.openSockets, rel.openSockets, unrel.openSockets);

    mlprintf(ourPos.x, ourPos.y+3, "%-18s%10d%10d%10d%11d%10d%10d",
      "bytes", all.bytesRecv, rel.bytesRecv, unrel.bytesRecv,
      all.bytesSent, rel.bytesSent, unrel.bytesSent);
    mlprintf(ourPos.x, ourPos.y+4, "%-18s%10d%10d%10d%11d%10d%10d",
      "bytes/sec", all.avgBytesRecv, rel.avgBytesRecv, unrel.avgBytesRecv,
      all.avgBytesSent, rel.avgBytesSent, unrel.avgBytesSent);
    mlprintf(ourPos.x, ourPos.y+5, "%-18s%10d%10d%10d%11d%10d%10d",
      "max b/s", all.maxAvgBytesRecv, rel.maxAvgBytesRecv, unrel.maxAvgBytesRecv,
      all.maxAvgBytesSent, rel.maxAvgBytesSent, unrel.maxAvgBytesSent);
  }

  /**
   * Redraws the global stats.
   */
  void updateGlobalStats() {
    ConnectionStats all = GNE::getGlobalStats();
    mlprintf(gpos.x, gpos.y, "%-18s%15s%15s", "global", "in:tot", "out:tot");
    mlprintf(gpos.x, gpos.y+1, "%-18s%15d%15d", "bytes",
      all.bytesRecv, all.bytesSent);
    mlprintf(gpos.x, gpos.y+2, "%-18s%15d%15d", "Rate",
      all.avgBytesRecv, all.avgBytesSent);
    mlprintf(gpos.x, gpos.y+3, "%-18s%15d%15d", "maxRate",
      all.maxAvgBytesRecv, all.maxAvgBytesSent);
  }

private:
  /**
   * Finds the first screen position used by conn.  If conn is NULL this is
   * OK -- it will find the first empty position.  Returns NULL on error.
   * sync should already be aquired here so we don't reacquire it.
   */
  Pos* findPos(Connection* conn) {
    for (int c = 0; c < positionsLength; ++c)
      if (positions[c].conn == conn)
        return &positions[c];
    //If we reach here then the loop failed to find a pos.
    return NULL;
  }

  Timer* timer;

  Mutex sync;
};
