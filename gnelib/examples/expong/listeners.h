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

//Our includes have code in it.  Normally this is a bad thing, but to simplify
//the makefile process, each example only has one .cpp.

//This include file includes the classes we have inherited from
//ConnectionListener and ServerConnectionListener.

/**
 * The PongClient is suitable for both sides of the connection, since this
 * implementation of pong is a very simple VERY trusting peer-to-peer network
 * game.
 *
 * It is also a PaddleListener so it can get events from the game logic to
 * know when the game state has changed, or a point has been scored.
 */
class PongClient : public ConnectionListener, public PaddleListener {
public:
  /**
   * The PongClient requires a paddle passed to be designated as its
   * associated network player.  If note is not NULL, then that CV is notifed
   * when a connection is made (this is for the server to sleep for a client
   * to enter).  The localPlayer is passed so that we may accredit him with
   * points when the remote paddle misses the ball.
   */
  PongClient(Player* RemotePlayer, Player* LocalPlayer)
    : conn(NULL), remotePlayer(RemotePlayer), localPlayer(LocalPlayer) {}

  ~PongClient() {}

  //This function is used by the server so it can get the ServerConnection
  //so it can delete it later.
  Connection* getConnection() {
    return conn;
  }

  void onDisconnect() {
  }

  void onExit() {
    mlprintf(0, 24, "Remote player quit.  Press q to exit.");
  }

  void onConnect(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();

    //Do connection negotiation here.  If it fails, onConnectFailure will be
    //called.  Any interaction we have will conn2 can throw an exception.
    //The SyncConnection does all of the error checking for us.

    //Create a CustomPacket which we will use to send and receive primitive
    //data types.  SyncConnection can only send GNE packet types, and a
    //CustomPacket wraps a RawPacket, allowing us to send RawPackets back and
    //forth.
    CustomPacket buf;

    //First, read from the server if there is a player slot open and we are
    //accepted.  If not, throw a connection refused error.
    gbool accepted;
    conn2 >> buf;
    buf.getData() >> accepted;
    if (!accepted) {
      conn2.disconnect();
      throw Error(Error::ConnectionRefused);
    }

    //If we are accepted, we should be receiving the remote player's name.
    conn2 >> buf;
    string remoteName;
    buf.getData() >> remoteName;
    remotePlayer->setName(remoteName);

    //Now we should send our name.
    buf.reset();
    buf.getData() << localPlayer->getName();
    conn2 << buf;

    //Now we are ready to play pong!
  }

  void onNewConn(SyncConnection& conn2) throw (Error) {
    conn = conn2.getConnection();

    CustomPacket buf;
    //Tell the client that they have been accepted
    buf.getData() << gTrue;
    conn2 << buf;
    //and send them our name.
    buf.reset();
    buf.getData() << localPlayer->getName();
    conn2 << buf;

    //Now we wait for the client's name.
    conn2 >> buf;

    string remoteName;
    buf.getData() >> remoteName;
    remotePlayer->setName(remoteName);
  }

  void onReceive() {
    Packet* next = conn->stream().getNextPacket();
    while (next != NULL) {
      int type = next->getType();
      if (type == PaddleMovement::ID) {
        //gnedbgo1(1, "paddleMoved %d received", ((PaddleMovement*)next)->newy);
        remotePlayer->paddle().changePosition(((PaddleMovement*)next)->newy);

      } else if (type == BallMissed::ID) {
        localPlayer->incrementScore();

      } else if (type == PingPacket::ID) {
        //We reply to ping requests, and check ping times on replies.
        PingPacket& ping = *((PingPacket*)next);
        if (ping.isRequest()) {
          ping.makeReply();
          conn->stream().writePacket(ping, true);
        } else {
          mlprintf(63, 24, "Ping: %ss", ping.getPing().toString().c_str());
        }

      } else {
        mlprintf(0, 24, "Unexpected packet type %d received!", type);
      }

      delete next;
      next = conn->stream().getNextPacket();
    }
  }

  //These next two functions are from us being a PaddleListener
  void paddleMoved(int newy) {
    //gnedbgo1(1, "paddleMoved %d sent", newy);
    //The paddle has moved.  Now let's tell the other side about that.
    PaddleMovement packet;
    packet.newy = newy;
    conn->stream().writePacket(packet, true);
  }

  void ballMissed() {
    //We missed the ball.  Now let's tell the other side about that.
    BallMissed packet;
    conn->stream().writePacket(packet, true);
  }

  void onFailure(const Error& error) {
    mlprintf(0, 24, "Socket failure: %s   ", error.toString().c_str());
    //No need to disconnect, this has already happened on a failure.
  }

  void onError(const Error& error) {
    mlprintf(0, 24, "Socket error: %s   ", error.toString().c_str());
    conn->disconnect();//For simplicity we treat even normal errors as fatal.
  }

  void onConnectFailure(const Error& error) {
    gout << "Connection to server failed." << endl;
    gout << "  GNE reported error: " << error << endl;
  }

private:
  Connection* conn;

  Player* remotePlayer;
  Player* localPlayer;
};

/**
 * This is a class that the server side uses to refuse a client if one is
 * already connected.
 */
class RefuseClient : public ConnectionListener {
public:
  void onNewConn(SyncConnection& conn2) throw (Error) {
    CustomPacket buf;
    //Tell the client that they have been refused
    buf.getData() << gFalse;
    conn2 << buf;
    conn2.disconnect();
    throw Error(Error::ConnectionRefused);
  }
private:
};

class OurListener : public ServerConnectionListener {
public:
  /**
   * This listener takes the params it needs to pass them onto the PongClient
   * to set up the game.
   */
  OurListener(Player* RemotePlayer, Player* LocalPlayer)
    : ServerConnectionListener(), remotePlayer(RemotePlayer),
    localPlayer(LocalPlayer), player(NULL), connecting(NULL), accept(true) {
  }

  virtual ~OurListener() {}

  void onListenFailure(const Error& error, const Address& from, ConnectionListener* listener) {
    sync.acquire();
    if (listener == connecting) {
      //Only display an error for our real player.  We don't want to see the
      //ConnectionRefused errors.
      gout << acquire << "Connection error: " << error << endl;
      gout << "  Error received from " << from << endl << release;
      connecting = NULL;
    }
    sync.broadcast();
    sync.release();

    //If listener is NULL, that is OK even.
    delete listener;
  }

  void onListenSuccess(ConnectionListener* listener) {
    sync.acquire();
    player = connecting;
    connecting = NULL;
    accept = false;
    sync.broadcast();
    sync.release();
  }

  void getNewConnectionParams(int& inRate, int& outRate,
                              bool& allowUnreliable,
                              ConnectionListener*& listener) {
    sync.acquire();

    inRate = outRate = 0;
    allowUnreliable = false;
    if (accept && !connecting) {
      //If no one is connecting and we are accepting connections
      listener = connecting = new PongClient(remotePlayer, localPlayer);
    } else {
      listener = new RefuseClient();
    }

    sync.release();
  }

  //waitForPlayer returns the connected player, or NULL if the connection
  //process was aborted by the user.
  PongClient* waitForPlayer() {
    sync.acquire();

    while (!player && !kbhit()) {
      //We wait for 250ms to recheck kbhit for pressed keys.
      sync.timedWait(250);
    }
    if (!player) {
      //We were woken up by a keypress, so refuse any further connections.
      accept = false;
      if (connecting) {
        //We have a client connecting when we pressed a key, and we have to
        //wait for that connection to complete so we can delete the connection
        //that we don't want anymore.
        while (connecting) {
          sync.wait();
        }
        //When connecting becomes NULL, check for a successful connect, and if
        //it was, close it.
        if (player) {
          player->getConnection()->disconnect();
          delete player->getConnection();
          delete player;
          player = NULL;
        }
      }
    }

    sync.release();

    return player;
  }

private:
  Player* remotePlayer;
  Player* localPlayer;

  //This variable will be non-null when there is a player, so we refuse any
  //other incoming connections.
  PongClient* player;

  //player will be stored here while he is connecting, then moved to player
  //when the connection was successful.
  PongClient* connecting;

  //If this is false, then the user canceled the connection process, we we
  //shouldn't even accept the first player.
  bool accept;

  //We use a CV because getNewConnectionParams might be called from
  //different threads, and we want to make sure only one client connects at a
  //time.  This protects the state of player.
  //The CV is notified when a new player arrives.
  ConditionVariable sync;
};
