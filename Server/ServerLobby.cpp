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
            addPlayer();
            FD_SET(newSock, &m_readfds);
            if(newSock >= m_numfds)
                m_numfds = newSock + 1;

            string num = to_string(m_comm->numPlayers() - 1);
            m_comm->sendPacket(newSock, HEADER_TYPE_INDEX, num.length(), num.c_str());
        }
    } while(!checkIfReady() || !tryCountdown());

    close(acceptSock);

    return true;
}

void ServerLobby::checkFds()
{
    bool reorder = false;
    auto player = m_comm->playerList();
    int i = 0;
    auto p = m_players.begin();
    auto prep = m_players.before_begin();

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
                p->ready = curReady[0];
                if(curReady[0])
                    cout << " Ready." << endl;
                else
                    cout << " No-go." << endl;
            }
            else if(res == 0 || errno != EWOULDBLOCK)
            {
                reorder = true;
                cout << "Player lost!" << endl;
                player = m_comm->removePlayerSocket(*player);
                prep = m_players.erase_after(prep);
                p = next(prep);
                continue;
            }
        }

        player++;
        p++;
        prep++;
        i++;
    }

    if(reorder)
    {
        // TODO: Send out new indices
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
        m_comm->broadcastPacket(HEADER_TYPE_READY, time.length(), time.c_str());

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

    for(auto p : m_players)
    {
        result &= p.ready;
    }

    return result;
}

void ServerLobby::addPlayer()
{
    auto it = m_players.begin();

    if(it == m_players.end())
    {
        m_players.emplace_front();
    }
    else
    {
        while(next(it) != m_players.end()) it++;
        m_players.emplace_after(it);
    }
}
