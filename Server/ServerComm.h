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
    void sendBytes(int outsock, void *bytes, int numBytes);

    void broadcastBytes(void *bytes, int numBytes);

    void addPlayerSocket(int playerSock);

    private:
    forward_list<int> m_players;
};

#endif
