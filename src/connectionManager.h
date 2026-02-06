#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class connectionManager{
    public:
        int connectToPeer(int port);
        int startServer(int port);
};



#endif