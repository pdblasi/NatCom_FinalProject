#ifndef __SERVER_COMM_H__
#define __SERVER_COMM_H__

#include <forward_list>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "Globals.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

using namespace std;

class ServerComm
{
    public:
    ServerComm();
    ~ServerComm();

    void* readPacket(int insock, char &msgType, int &msgLen);
    void sendPacket(int outsock, const char &msgCode, const int &msgLen, const void *message);
    void broadcastPacket(const char &msgCode, const int &msgLen, const void *message);

    void addPlayerSocket(int playerSock);
    forward_list<int>::iterator removePlayerSocket(int playerSock);
    forward_list<int>::iterator playerList();
    int numPlayers();

    private:
    forward_list<int> m_players;
    int m_numPlayers = 0;
};

#endif
