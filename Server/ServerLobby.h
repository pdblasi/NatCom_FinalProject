#ifndef __SERVER_LOBBY_H__
#define __SERVER_LOBBY_H__

#include <sys/socket.h>
#include "ServerComm.h"

using namespace std;

class ServerLobby
{
    public:
    ServerLobby();
    ~ServerLobby();

    void setUpGame(ServerComm &comm);

    private:
};

#endif
