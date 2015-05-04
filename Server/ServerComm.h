#ifndef __SERVER_COMM_H__
#define __SERVER_COMM_H__

#include <stdlib.h>
#include <sys/socket.h>

class ServerComm
{
    public:
    void* getBytes(int insock, int numBytes);
    void sendBytes(int outsock, void *bytes, int numBytes);

    private:
};

#endif
