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
  PongClient(Player* RemotePlayer, Player* LocalPlayer, ConditionVariable* note)
    : conn(NULL), remotePlayer(RemotePlayer), localPlayer(LocalPlayer),
    notify(note) {}

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
    if (!accepted)
      throw Error(Error::ConnectionRefused);

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

    //Set ourself as a player, and notify the server code that a player has
    //arrived.
    notify->acquire();
    remotePeer = this;
    if (notify)
      notify->broadcast();
    notify->release();
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
    mlprintf(0, 0, "Connection to server failed.   ");
    mlprintf(0, 1, "  GNE reported error: %s   ", error.toString().c_str());
  }

private:
  Connection* conn;

  Player* remotePlayer;
  Player* localPlayer;

  ConditionVariable* notify;
};

class OurListener : public ServerConnectionListener {
public:
  /**
   * This listener takes the params it needs to pass them onto the PongClient
   * to set up the game.
   */
  OurListener(Player* RemotePlayer, Player* LocalPlayer, ConditionVariable* note)
    : ServerConnectionListener(), remotePlayer(RemotePlayer),
    localPlayer(LocalPlayer), notify(note) {
  }

  virtual ~OurListener() {}

  void onListenFailure(const Error& error, const Address& from, ConnectionListener* listener) {
    gout << acquire << "Connection error: " << error << endl;
    gout << "  Error received from " << from << endl << release;
    //If listener is NULL, that is OK even.
    delete listener;
  }

  void getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener) {
    inRate = outRate = 0;
    listener = new PongClient(remotePlayer, localPlayer, notify);
  }

private:
  Player* remotePlayer;
  Player* localPlayer;

  ConditionVariable* notify;
};
