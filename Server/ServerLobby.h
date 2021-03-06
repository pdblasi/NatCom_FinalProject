#ifndef __SERVER_LOBBY_H__
#define __SERVER_LOBBY_H__

#include <iostream>

#include <fcntl.h>
#include <forward_list>
#include <string>
#include <string.h>
#include "Globals.h"

#ifdef _WIN32
    #include <sstream>
    #include <winsock2.h>
    #include <winbase.h>
    #include <windows.h>
    typedef int socklen_t;
    #define SLEEP 1000
#else
	#include <strings.h>
	#include <unistd.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #define SLEEP 1
#endif

#include "Globals.h"
#include "ServerComm.h"

using namespace std;

class ServerLobby
{
    public:
    ServerLobby(ServerComm *comm);
    ~ServerLobby();

    bool acceptMode();

    struct connectedPlayer
    {
        bool ready = false;
        char *color = NULL;
    };

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
    string getPlayerList();
    void updatePlayerIndices();
};

#endif
