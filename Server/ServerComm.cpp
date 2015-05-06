#include "ServerComm.h"

void* ServerComm::getBytes(int insock, int numBytes)
{
    char *buffer = new char[numBytes];
    int result = 0, recvd = 0;

    do
    {
        errno = 0;
        result = recv(insock, (void*)buffer, numBytes, 0);
        recvd += result;
    } while(recvd < numBytes && (result > 0 || errno == EWOULDBLOCK));

    if(recvd != numBytes)
    {
        delete [] buffer;
        buffer = NULL;
    }

    return buffer;
}

void ServerComm::sendBytes(int outsock, const void *bytes, int numBytes)
{
    char *sendBuffer = (char*)bytes;
    int result = 0, sent = 0;

    do
    {
        errno = 0;
        result = send(outsock, &sendBuffer[sent], numBytes-sent, 0);
        sent += result;
    } while(sent < numBytes && (result > 0 || errno == EWOULDBLOCK));
}

void ServerComm::broadcastBytes(const void *bytes, int numBytes)
{
    for(auto player : m_players)
    {
        sendBytes(player, bytes, numBytes);
    }
}

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
}

void ServerComm::removePlayerSocket(int playerSock)
{
    m_players.remove(playerSock);
}

forward_list<int>::iterator ServerComm::playerList()
{
    return m_players.begin();
}
