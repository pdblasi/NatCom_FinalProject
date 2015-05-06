#include "ServerLobby.h"

ServerLobby::ServerLobby(ServerComm *comm)
{
    m_comm = comm;
}

bool ServerLobby::acceptMode()
{
    // Create variables
    int acceptSock = socket(AF_INET, SOCK_STREAM, O_NONBLOCK);
    struct sockaddr_in serv_addr, cli_addr;

    // Set up serv_addr
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(35015);

    // Bind to accept socket & set to listening mode
    bind(acceptSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(acceptSock, 10);

    // Accept incoming connections!
    socklen_t clilen = sizeof(cli_addr);
    do
    {
        int newSock = accept(acceptSock, (struct sockaddr *)&cli_addr, &clilen);
        
        m_comm->addPlayerSocket(newSock);
        addPlayerReadiness();
    } while(!checkIfReady() && !tryCountdown());

    close(acceptSock);
}

bool ServerLobby::tryCountdown()
{
    int timer = 10;

    do
    {
        string time = to_string(timer);
        m_comm->broadcastBytes(time.c_str(), time.length());
        sleep(1);
    } while(timer >= 0 && checkIfReady());

    return (timer == 0);
}

bool ServerLobby::checkIfReady()
{
    bool result = readiness.begin() != readiness.end();
    auto player = m_comm->playerList();
    auto ready = readiness.begin();
    auto tracker = readiness.before_begin();

    while(ready != readiness.end())
    {
        errno = 0;
        bool curReady[1];
        int res = recv(*player, (void*)curReady, 1, 0);
        if(res == 1)
        {
            *ready = curReady[0];
        }
        else if(res == 0 || errno != EWOULDBLOCK)
        {
            m_comm->removePlayerSocket(*player);
            readiness.erase_after(tracker);

            ready = readiness.begin();
            player = m_comm->playerList();
            continue;
        }

        result &= *ready;
        
        tracker++;
        ready++;
        player++;
    }

    return result;
}

void ServerLobby::addPlayerReadiness()
{
    auto it = readiness.begin();

    if(it == readiness.end())
    {
        readiness.push_front(false);
    }
    else
    {
        while(next(it) != readiness.end()) it++;
        readiness.insert_after(it, false);
    }
}
