#include "ServerLobby.h"

#ifdef _WIN32
void bzero(char *arr, size_t size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		arr[i] = 0;
	}
}
#endif

ServerLobby::ServerLobby(ServerComm *comm)
{
    m_comm = comm;
}

ServerLobby::~ServerLobby()
{
    for(auto p : m_players)
    {
        delete [] p.color;
    }
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
    if(bind(acceptSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        cout << "Unable to bind socket. " << errno << endl;
        return false;
    }
    if(listen(acceptSock, 10) != 0)
    {
        cout << "Unable to listen on socket. " << errno << endl;
        return false;
    }

    m_numfds = acceptSock+1;
	FD_ZERO(&m_readfds);
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

        cout << "Do read!" << endl;

        checkFds();
        
        if(FD_ISSET(acceptSock, &m_tempreadfds) != 0)
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
            updatePlayerIndices();
        }
    } while(!checkIfReady() || !tryCountdown());

    #ifdef _WIN32
        closesocket(acceptSock);
    #else
        close(acceptSock);
    #endif

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
            char msgType;
            int msgLen;
            void *msg = m_comm->readPacket(*player, msgType, msgLen);

            if(msg == NULL)
            {
                reorder = true;
                player = m_comm->removePlayerSocket(*player);
                prep = m_players.erase_after(prep);
                p = next(prep);
                continue;
            }

            reorder = true;

            if(msgType == HEADER_TYPE_READY)
            {
                bool *curReady = (bool*)msg;
                p->ready = curReady[0];
            }
            else if(msgType == HEADER_TYPE_PLAYER_UPDATES)
            {
                strncpy_s(p->color, msgLen+1, (char*)msg, msgLen);
            }

            delete [] (char*)msg;
        }

        player++;
        p++;
        prep++;
        i++;
    }

    if(reorder)
    {
        updatePlayerIndices();
    }
}

string ServerLobby::getPlayerList()
{
    static string colors[] = {"#FF0000", "#00FF00", "#0000FF",
                              "#FFFF00", "#00FFFF", "#FF00FF",
                              "#F00FF0", "#0FF00F", "#99AA88",
                              "#FF55AA"};
    string list = "[";
    auto player = m_players.begin();

    for(int i = 0; i < m_comm->numPlayers(); i++, player++)
    {
        if(i > 0)
            list += ",";

        list += "(";
        list += to_string(i);
        list += ",\"";
        if(player->color == NULL)
        {
            player->color = new char[8];
            strncpy_s(player->color, 9, colors[i].c_str(), 8);
        }
        list += string(player->color);
        list += "\")";
    }
    list += "]";

    cout << list << endl;

    return list;
}

void ServerLobby::updatePlayerIndices()
{
    string list = getPlayerList();
    auto it = m_comm->playerList();

    for(int i = 0; i < m_comm->numPlayers(); i++, it++)
    {
        string msg = "[" + to_string(i) + "," + list + "]";
        m_comm->sendPacket(*it, HEADER_TYPE_PLAYER_UPDATES, msg.length(), (void*)msg.c_str());
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
        cout << time << endl;

        m_tempreadfds = m_readfds;
#ifdef _WIN32
		Sleep(SLEEP);
#else
        sleep(SLEEP);
#endif
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
