#ifndef __SERVER_LOBBY_H__
#define __SERVER_LOBBY_H__

#include <fcntl.h>
#include <forward_list>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "ServerComm.h"

using namespace std;

class ServerLobby
{
    public:
    ServerLobby(ServerComm *comm);

    bool acceptMode();
    bool tryCountdown();
    bool checkIfReady();

    private:
    ServerComm *m_comm;
    forward_list<bool> readiness;

    void addPlayerReadiness();
};

#endif
