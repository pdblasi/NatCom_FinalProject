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
        cout << "Woohoo!" << endl;
    }
    else
    {
        cout << "Hmm..." << endl;
    }

    return 0;
}
