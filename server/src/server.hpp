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
    private:
        TCPsocket sd, csd;
        IPaddress ip, *remoteIP; 
        data ServerData;
};