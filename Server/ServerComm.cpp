#include "ServerComm.h"

void* ServerComm::getBytes(int insock, int numBytes)
{
    char *buffer = new char[numBytes];
    int result = recv(insock, (void*)buffer, numBytes, MSG_WAITALL);

    if(result != numBytes)
    {
        delete [] buffer;
        buffer = NULL;
    }

    return buffer;
}

void ServerComm::sendBytes(int outsock, void *bytes, int numBytes)
{
    char *sendBuffer = (char*)bytes;
    int sentBytes = 0, nextChunk = 0;
    
    while(sentBytes < numBytes && nextChunk >= 0)
    {
        nextChunk = send(outsock, &sendBuffer[sentBytes], numBytes-sentBytes, 0);
        sentBytes += nextChunk;
    }
}
