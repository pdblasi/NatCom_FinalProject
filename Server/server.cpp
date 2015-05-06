#include "ServerComm.h"
#include "ServerLobby.h"
#include "ServerEngine.h"

#include <iostream>

using namespace std;

int main()
{
    ServerComm comm;
    ServerLobby lobby(&comm);

    if(lobby.acceptMode())
    {
        ServerEngine engine(&comm);

        while(!engine.generateNextStep());
    }
    
    return 0;
}
