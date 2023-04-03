#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "SDL_net.h"
#include "data.hpp"

class Server{
    public:
        Server(const uint16_t &port);
        ~Server();
        data *Run();

        uint32_t time = 0;

    private:
        void CloseSocket(int index);
        int AcceptSocket(int index);
        uint64_t Hash_Address(uint32_t host, uint16_t port);
        TCPsocket server;
        SDLNet_SocketSet socket_set;
        std::unordered_map<uint64_t, TCPsocket> sockets;

        data *ServerData;
};