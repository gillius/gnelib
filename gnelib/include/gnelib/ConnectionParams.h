#ifndef _CONNECTIONPARAMS_H_
#define _CONNECTIONPARAMS_H_

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
class ConnectionListener;
class PacketFeeder;

/**
 * A class to encapsulate parameters about a connection.  The parameter list
 * was starting to get very complex, and I knew in the future that I wanted
 * to expand the list of parameters, and perhaps change the defaults.  This
 * class when constructed sets everything to defaults, so change only the
 * parameters that you care about.
 */
//##ModelId=3CBE05D40261
class ConnectionParams {
public:
  /**
   * Creates a new ConnectionParams object using the default values, and
   * optionally setting the listener property.  A non-NULL listener is always
   * needed by the time it comes active, so it is almost always specified.
   */
  //##ModelId=3CBE05D4035C
  ConnectionParams(ConnectionListener* Listener = NULL);

  /**
   * Returns true if any of the parameters are invalid or out of range.
   */
  //##ModelId=3CBE05D4035E
  bool checkParams() const;

  /**
   * Returns the result of checkParams.
   */
  //##ModelId=3CBE05D40360
  operator bool() const { return checkParams(); }

  /**
   * The ConnectionListener that will receive events from the Connection.
   * Valid value is any non-NULL pointer.
   */
  //##ModelId=3CBE05D40362
  void setListener(ConnectionListener* Listener);

  //##ModelId=3CBE05D40364
  ConnectionListener* getListener() const;

  /**
   * The PacketFeeder receives onLowPackets events which are entirely
   * independant of the serialized event queue of events sent to the
   * ConnectionListener.  The default is NULL, and any point is valid.
   * @see PacketStream::setFeeder
   */
  //##ModelId=3CE60C4803CD
  void setFeeder(PacketFeeder* Feeder);

  //##ModelId=3CE60C4803CF
  PacketFeeder* getFeeder() const;

  /**
   * Sets the timeout of the PacketFeeder for this Connection.  Default is 0
   * meaning no timeouts generated.  Valid values are 0 and positive integers
   * given in milliseconds.
   * @see PacketStream::setFeederTimeout
   */
  //##ModelId=3CE60C4803D1
  void setFeederTimeout(int FeederTimeout);

  //##ModelId=3CE60C4803D3
  int getFeederTimeout() const;

  /**
   * The low packet threshold for the PacketFeeder.  Default value is 0, 0 or
   * any positive integer is valid.
   * @see PacketStream::setLowPacketThreshold
   */
  //##ModelId=3CE60C4803D5
  void setLowPacketThreshold(int limit);

  //##ModelId=3CE60C4803D7
  int getLowPacketThreshold() const;

  /**
   * Sets the timeout for this connection in milliseconds.  A value of 0
   * signifies that no timeouts should occur.  Values less than 0 are invalid.
   * @see Connection::setTimeout
   */
  //##ModelId=3CC4E33800D3
  void setTimeout(int ms);

  /**
   * Gets the timeout.
   */
  //##ModelId=3CC4E33800D5
  int getTimeout() const;

  /**
   * The maximum rate we will transmit in bytes per second.  If this is 0,
   * then the rate is unlimited.  Valid values are 0 or a positive integer.
   *
   * The default out rate is 0 (unlimited).
   */
  //##ModelId=3CBE05D40366
  void setOutRate(int OutRate);

  //##ModelId=3CBE05D40368
  int getOutRate() const;

  /**
   * The maximum rate we allow the sender to send to us in bytes per second.
   * If this is 0, then the requested incoming rate has no bounds.  Valid
   * values are 0 or a positive integer.
   *
   * The default in rate is 0 (unlimited).
   */
  //##ModelId=3CBE05D4036A
  void setInRate(int InRate);

  //##ModelId=3CBE05D4036C
  int getInRate() const;

  /**
   * A shortcut to set both rates at the same time.
   */
  //##ModelId=3CBE05D4036E
  void setRates(int OutRate, int InRate);

  /**
   * For client-side connections, this will set a local port, if you desire,
   * although most of the time you will want to keep this at its default value
   * of 0 to let the OS pick the local port for you.  Valid values are
   * [0..65535].
   *
   * This option is ignored for ServerConnection.
   */
  //##ModelId=3CBE05D40371
  void setLocalPort(int LocalPort);

  //##ModelId=3CBE05D40373
  int getLocalPort() const;

  /**
   * Set this to true if you want to be able to send unreliable packets over
   * this connection.  Setting this to false will provide a reliable-only
   * connection, so packets sent unreliably will instead be sent reliably.
   *
   * An unreliable connection is created only when both sides agree to open
   * one by setting this parameter to true.  A reliable connection is
   * available regardless of this setting.
   *
   * The default for unreliable is false.
   */
  //##ModelId=3CBE05D40375
  void setUnrel(bool set);

  //##ModelId=3CBE05D40377
  bool getUnrel() const;

private:
  //##ModelId=3CBE05D4034F
  ConnectionListener* listener;

  //##ModelId=3CE60C4803C7
  PacketFeeder* feeder;

  //##ModelId=3CE60C4803CB
  int feederTimeout;

  //##ModelId=3CE60C4803CC
  int feederThresh;

  //##ModelId=3CC4E33800C9
  int timeout;

  //##ModelId=3CBE05D40358
  int outRate;

  //##ModelId=3CBE05D40359
  int inRate;

  //##ModelId=3CBE05D4035A
  int localPort;

  //##ModelId=3CBE05D4035B
  bool unrel;
};

}

#endif
