#include "ServerComm.h"

ServerComm::ServerComm()
{
    #ifdef _WIN32
        WSADATA wsaData;
        if(WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
        {
            cout << "WSAStartup failed." << endl;
            exit(1);
        }
    #endif
}

ServerComm::~ServerComm()
{
    for(auto player : m_players)
    {
        #ifdef _WIN32
            closesocket(player);
        #else
            close(player);
        #endif
    }
}

// ~~~ Message Transmission Protocol ~~~

void* ServerComm::readPacket(int insock, char &msgType, int &msgLen)
{
    int read, readTot;
    char type[1];
    char length[4];
    char *msg;

    // Read msg type
    read = recv(insock, type, 1, 0);
    if(read < 1)
        return NULL;
    msgType = type[0];

    // Read msg length
    readTot = 0;
    while(readTot < 4)
    {
        read = recv(insock, length+readTot, 4-readTot, 0);
        if(read < 1)
            return NULL;
        readTot += read;
    }
    msgLen = *((int*)length);

    // Read msg
    readTot = 0;
    msg = new char[msgLen];
    while(readTot < msgLen)
    {
        read = recv(insock, msg+readTot, msgLen-readTot, 0);
        if(read < 1)
        {
            delete [] msg;
            return NULL;
        }
        readTot += read;
    }

    return msg;
}

void ServerComm::sendPacket(int outsock, const char &msgCode, const int &msgLen, const void *message)
{
    int sent, totSent;

    send(outsock, &msgCode, 1, 0);

    totSent = 0;
    while(totSent < 4)
    {
        sent = send(outsock, (char*)&msgLen+totSent, 4-totSent, 0);
        if(sent < 1)
            return;
        totSent += sent;
    }

    totSent = 0;
    while(totSent < msgLen)
    {
        sent = send(outsock, (char*)message+totSent, msgLen-totSent, 0);
        if(sent < 1)
            return;
        totSent += sent;
    }
}

void ServerComm::broadcastPacket(const char &msgCode, const int &msgLen, const void *message)
{
    for(auto player : m_players)
    {
        sendPacket(player, msgCode, msgLen, message);
    }
}

// ~~~ Socket Tracking Methods ~~~

void ServerComm::addPlayerSocket(int playerSock)
{
    forward_list<int>::iterator it = m_players.begin();

    if(it == m_players.end())
    {
        m_players.push_front(playerSock);
    }
    else
    {
        while(next(it) != m_players.end()) it++;
        m_players.insert_after(it, playerSock);
    }
    m_numPlayers++;
}

forward_list<int>::iterator ServerComm::removePlayerSocket(int playerSock)
{
    m_numPlayers--;
    
    auto erit = m_players.before_begin();

    while(*(next(erit)) != playerSock)
    {
        erit++;
    }

    return m_players.erase_after(erit);
}

forward_list<int>::iterator ServerComm::playerList()
{
    return m_players.begin();
}

int ServerComm::numPlayers()
{
    return m_numPlayers;
}
