#ifndef __SERVER_LOBBY_H__
#define __SERVER_LOBBY_H__

#include <iostream>

#include <fcntl.h>
#include <forward_list>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "Globals.h"
#include "ServerComm.h"

using namespace std;

class ServerLobby
{
    struct connectedPlayer
    {
        bool ready;
        char *color;
    };

    public:
    ServerLobby(ServerComm *comm);

    bool acceptMode();

    private:
    ServerComm *m_comm;
    forward_list<connectedPlayer> m_players;
    int m_numfds;
    fd_set m_readfds;
    fd_set m_tempreadfds;

    bool tryCountdown();
    bool checkIfReady();
    void addPlayer();
    void checkFds();
};

#endif
