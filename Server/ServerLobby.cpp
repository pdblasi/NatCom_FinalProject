#include "ServerLobby.h"

ServerLobby::ServerLobby(ServerComm *comm)
{
    m_comm = comm;
}

bool ServerLobby::acceptMode()
{
    // Create variables
    int acceptSock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, cli_addr;

    // Set up serv_addr
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(35015);

    // Bind to accept socket & set to listening mode
    bind(acceptSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(acceptSock, 10);

    m_numfds = acceptSock+1;
    FD_SET(acceptSock, &m_readfds);

    // Accept incoming connections!
    socklen_t clilen = sizeof(cli_addr);
    do
    {
        m_tempreadfds = m_readfds;

        select(m_numfds,
               &m_tempreadfds,
               NULL,
               NULL,
               NULL);

        checkFds();
        
        if(FD_ISSET(acceptSock, &m_tempreadfds) == true)
        {
            int newSock = accept(acceptSock,
                                 (struct sockaddr*)&cli_addr,
                                 &clilen);

            cout << "New Player!" << endl;
            m_comm->addPlayerSocket(newSock);
            addPlayerReadiness();
            FD_SET(newSock, &m_readfds);
            if(newSock >= m_numfds)
                m_numfds = newSock + 1;
        }

        sleep(0.1);
    } while(!checkIfReady() || !tryCountdown());

    close(acceptSock);

    return true;
}

void ServerLobby::checkFds()
{
    auto player = m_comm->playerList();
    int i = 0;
    auto ready = readiness.begin();
    auto tracker = readiness.before_begin();
    while(i < m_comm->numPlayers())
    {
        if(FD_ISSET(*player, &m_tempreadfds))
        {
            errno = 0;
            bool curReady[1];
            int res = recv(*player, (void*)curReady, 1, 0);
            if(res == 1)
            {
                cout << "Readiness read!" << endl;
                *ready = curReady[0];
                if(*ready)
                    cout << " Ready." << endl;
                else
                    cout << " No-go." << endl;
            }
            else if(res == 0 || errno != EWOULDBLOCK)
            {
                cout << "Player lost!" << endl;
                m_comm->removePlayerSocket(*player);
                readiness.erase_after(tracker);

                ready = readiness.begin();
                player = m_comm->playerList();
                continue;
            }
        }

        player++;
        i++;
        ready++;
    }
}

bool ServerLobby::tryCountdown()
{
    int timer = 11;
    struct timeval tv;

    cout << "Trying countdown..." << endl;
    tv.tv_sec = 0;
    tv.tv_usec = 0;


    do
    {
        timer--;
        string time = to_string(timer);
        m_comm->broadcastBytes(time.c_str(), time.length());

        m_tempreadfds = m_readfds;
        sleep(1);
        int r = select(m_numfds,
                        &m_tempreadfds,
                        NULL,
                        NULL,
                        &tv);
        if(r > 0)
            checkFds();
    } while(timer > 0 && checkIfReady());

    return (timer == 0);
}

bool ServerLobby::checkIfReady()
{
    if(m_comm->numPlayers() == 0)
        return false;

    bool result = true;

    for(auto ready : readiness)
    {
        result &= ready;
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
