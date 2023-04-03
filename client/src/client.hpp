#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "SDL_net.h"
#include "data.hpp"

class Client{
    public:
        Client(uint16_t port, char *ip);
        ~Client();
        recieved *Recieve();
        void Send(sent *data);

        IPaddress server_address;

    private:
        uint64_t Hash_Address(uint32_t host, uint16_t port);
        TCPsocket socket;

        recieved *ServerData;
        sent *ClientData;
};