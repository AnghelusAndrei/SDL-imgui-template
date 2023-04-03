#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL_net.h"
#include "data.hpp"

class Server{
    public:
        Server();
        ~Server();
        data Run();

        uint32_t time = 0;
    private:
        void CloseSocket(int index);
        int AcceptSocket(int index);
        TCPsocket server;
        SDLNet_SocketSet socket_set;
        TCPsocket sockets[MAX_SOCKETS] = {};
        IPaddress ip; 
        data ServerData;
};