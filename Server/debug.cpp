#include "ServerComm.h"
#include "ServerLobby.h"
#include "ServerEngine.h"

#include <iostream>

using namespace std;

int main()
{
    ServerComm comm;
    ServerEngine engine(&comm);
    int i = 0;

    while(!engine.generateNextStep() && i < 25001)
    {
        if(i % 5000 == 0)
            engine.printCurrentState(i);
        i++;
    }
    
    return 0;
}
