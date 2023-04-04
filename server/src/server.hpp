#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "SDL_net.h"
#include "data.hpp"

class Server{
    public:
        Server(const uint16_t &port_);
        ~Server();
        data *Run();

        //data that needs to be passed to the window 
        char *debug;
        bool debug_init = false;
        bool debug_error = false;

        //server data
        IPaddress ip; 
        uint16_t port;
        uint16_t num_ready = 0;

        data *ServerData;

    private:
        uint64_t Hash_Address(uint32_t host, uint16_t port);

        TCPsocket server;
        SDLNet_SocketSet socket_set;
        std::unordered_map<uint64_t, TCPsocket> sockets;

        uint32_t time = 0;

    friend class App;
};