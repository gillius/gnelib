#ifndef SERVERCONNECTIONCREATOR_H_INCLUDED_C4FE3BD2
#define SERVERCONNECTIONCREATOR_H_INCLUDED_C4FE3BD2

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
class ServerConnection;

/**
 * A helper class so that the ServerConnectionListener can create your custom
 * class.  Overload the create function to return a new copy of your custom
 * class.  You will eventually receive it through onNewConn.
 */
//##ModelId=3B0753800348
class ServerConnectionCreator {
public:
  //##ModelId=3B0753810295
  virtual ~ServerConnectionCreator();

  /**
   * Overload this function to create your own ServerConnection child class.
   * The parameters should be used to initalize the ServerConnection.
   */
  //##ModelId=3B0753810297
  virtual ServerConnection* create(int outRate, int inRate, NLsocket rsocket2) = 0;

};

}
#endif /* SERVERCONNECTIONCREATOR_H_INCLUDED_C4FE3BD2 */


