#ifndef __SERVER_COMM_H__
#define __SERVER_COMM_H__

#include <forward_list>
#include <stdlib.h>
#include <sys/socket.h>

using namespace std;

class ServerComm
{
    public:
    void* getBytes(int insock, int numBytes);
    void sendBytes(int outsock, const void *bytes, int numBytes);

    void broadcastBytes(const void *bytes, int numBytes);

    void addPlayerSocket(int playerSock);
    void removePlayerSocket(int playerSock);
    forward_list<int>::iterator playerList();
    int numPlayers();

    private:
    forward_list<int> m_players;
    int m_numPlayers = 0;
};

#endif
