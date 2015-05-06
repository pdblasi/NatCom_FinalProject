#include "ServerComm.h"
#include "ServerLobby.h"
#include "ServerEngine.h"

#include <iostream>

using namespace std;

int main()
{
    ServerComm comm;
    ServerLobby lobby(&comm);

    lobby.acceptMode();

    ServerEngine engine(&comm);


    
    return 0;
}
