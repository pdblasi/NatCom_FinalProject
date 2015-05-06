#ifndef __SERVER_COMM_H__
#define __SERVER_COMM_H__

#include <forward_list>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

class ServerComm
{
    public:
    ~ServerComm();

    void* readPacket(int insock, char &msgType);
    void sendPacket(int outsock, char msgCode, int msgLen, const void *message);
    void broadcastPacket(char msgCode, int msgLen, const void *message);

    void addPlayerSocket(int playerSock);
    forward_list<int>::iterator removePlayerSocket(int playerSock);
    forward_list<int>::iterator playerList();
    int numPlayers();

    private:
    forward_list<int> m_players;
    int m_numPlayers = 0;
};

#endif
