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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/ConnectionParams.h"

namespace GNE {

//##ModelId=3CBE05D4035C
ConnectionParams::ConnectionParams(ConnectionListener* Listener)
: listener(Listener), feeder(NULL), feederTimeout(0), feederThresh(0),
timeout(0), outRate(0), inRate(0), localPort(0), unrel(false) {
}

//##ModelId=3CBE05D4035E
bool ConnectionParams::checkParams() const {
  return (outRate < 0 || inRate < 0 || localPort < 0 || localPort > 65535
    || listener == NULL || timeout < 0 || feederTimeout < 0
    || feederThresh < 0);
}

//##ModelId=3CBE05D40362
void ConnectionParams::setListener(ConnectionListener* Listener) {
  listener = Listener;
}

//##ModelId=3CBE05D40364
ConnectionListener* ConnectionParams::getListener() const {
  return listener;
}

//##ModelId=3CE60C4803CD
void ConnectionParams::setFeeder(PacketFeeder* Feeder) {
  feeder = Feeder;
}

//##ModelId=3CE60C4803CF
PacketFeeder* ConnectionParams::getFeeder() const {
  return feeder;
}

//##ModelId=3CE60C4803D1
void ConnectionParams::setFeederTimeout(int FeederTimeout) {
  feederTimeout = feederTimeout;
}

//##ModelId=3CE60C4803D3
int ConnectionParams::getFeederTimeout() const {
  return feederTimeout;
}

//##ModelId=3CE60C4803D5
void ConnectionParams::setLowPacketThreshold(int limit) {
  feederThresh = limit;
}

//##ModelId=3CE60C4803D7
int ConnectionParams::getLowPacketThreshold() const {
  return feederThresh;
}

//##ModelId=3CC4E33800D3
void ConnectionParams::setTimeout(int ms) {
  timeout = ms;
}

//##ModelId=3CC4E33800D5
int ConnectionParams::getTimeout() const {
  return timeout;
}

//##ModelId=3CBE05D40366
void ConnectionParams::setOutRate(int OutRate) {
  outRate = OutRate;
}

//##ModelId=3CBE05D40368
int ConnectionParams::getOutRate() const {
  return outRate;
}

//##ModelId=3CBE05D4036A
void ConnectionParams::setInRate(int InRate) {
  inRate = InRate;
}

//##ModelId=3CBE05D4036C
int ConnectionParams::getInRate() const {
  return inRate;
}

//##ModelId=3CBE05D4036E
void ConnectionParams::setRates(int OutRate, int InRate) {
  setOutRate(OutRate);
  setInRate(InRate);
}

//##ModelId=3CBE05D40371
void ConnectionParams::setLocalPort(int LocalPort) {
  localPort = LocalPort;
}

//##ModelId=3CBE05D40373
int ConnectionParams::getLocalPort() const {
  return localPort;
}

//##ModelId=3CBE05D40375
void ConnectionParams::setUnrel(bool set) {
  unrel = set;
}

//##ModelId=3CBE05D40377
bool ConnectionParams::getUnrel() const {
  return unrel;
}

}