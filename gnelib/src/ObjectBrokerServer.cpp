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
#include "../include/gnelib/ObjectBrokerPacket.h"
#include "../include/gnelib/ObjectCreationPacket.h"
#include "../include/gnelib/ObjectUpdatePacket.h"
#include "../include/gnelib/ObjectDeathPacket.h"
#include "../include/gnelib/ObjectBrokerServer.h"
#include "../include/gnelib/NetworkObject.h"
#include "../include/gnelib/Mutex.h"

namespace GNE {

ObjectBrokerServer::ObjectBrokerServer()
: ids( new bool[65536] ), nextId( 1 ), numObj( 0 ) {
  for ( int i=0; i < 65536; ++i ) {
    ids[i] = false;
  }
}

ObjectBrokerServer::~ObjectBrokerServer() {
  delete[] ids;
}

int ObjectBrokerServer::numObjects() const {
  return numObj;
}

ObjectCreationPacket::pointer
ObjectBrokerServer::getCreationPacket( NetworkObject& obj ) {

  if ( !obj.hasValidId() )
    obj.setObjectId( getNextId() );
#ifdef _DEBUG
  else {
    LockMutex lock(sync);
    assert( ids[ obj.getObjectId() ] );
  }
#endif

  Packet* packet = obj.createCreationPacket();
  assert( packet != NULL );
  ObjectCreationPacket* ret =
    new ObjectCreationPacket( obj.getObjectId(), *packet );
  delete packet;

  return ObjectCreationPacket::pointer( ret );
}

ObjectUpdatePacket::pointer ObjectBrokerServer::getUpdatePacket(
  NetworkObject& obj, const void* param ) {
#ifdef _DEBUG
  assert( obj.hasValidId() );
  sync.acquire();
  assert( ids[ obj.getObjectId() ] );
  sync.release();
#endif

  Packet* packet = obj.createUpdatePacket( param );
  assert( packet != NULL );
  ObjectUpdatePacket* ret =
    new ObjectUpdatePacket( obj.getObjectId(), *packet );
  delete packet;

  return ObjectUpdatePacket::pointer( ret );
}

ObjectUpdatePacket::pointer
ObjectBrokerServer::getUpdatePacket( NetworkObject& obj ) {
  return getUpdatePacket( obj, NULL );
}

ObjectDeathPacket::pointer
ObjectBrokerServer::getDeathPacket( NetworkObject& obj ) {
#ifdef _DEBUG
  assert( obj.hasValidId() );
  sync.acquire();
  assert( ids[ obj.getObjectId() ] );
  sync.release();
#endif

  Packet* packet = obj.createDeathPacket();
  ObjectDeathPacket* ret =
    new ObjectDeathPacket( obj.getObjectId(), packet );
  delete packet;

  return ObjectDeathPacket::pointer();
}

void ObjectBrokerServer::deregisterObject( NetworkObject& obj ) {
  assert( obj.hasValidId() );

  sync.acquire();
  int oldId = obj.getObjectId();
  assert( ids[ oldId ] );
  ids[ oldId ] = false;
  --numObj;
  obj.setObjectId( -1 );
  sync.release();

  obj.onDeregistration( oldId );
}

int ObjectBrokerServer::getNextId() {
  assert( ids != NULL );

  LockMutex lock(sync);
  if ( numObj >= 65535 )
    return -1;

  //Find the next valid ID
  while ( ids[nextId] ) {
    ++nextId;
    if ( nextId >= 65536 )
      nextId = 1;
  }

  //Mark it used and return it
  ids[nextId] = true;
  ++numObj;
  return nextId;
}
  
} //namespace GNE
