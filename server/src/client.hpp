#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "SDL.h"
#include "SDL_net.h"
#include "data.hpp"
#include "logger/logger.hpp"

class Client{
public:
    Client();
    Client(Log *log_, TCPsocket socket_);
    ~Client();

    bool connection = true;

    bool c_response = false;
    bool s_response = false;

    TCPsocket socket;
    SDLNet_SocketSet *socket_set;
    
    client_package *c_package;
    server_package *s_package;
    uint16_t port;
    uint32_t host;

    bool c_package_action = false;
    bool s_package_action = false;

    //package size
    const size_t s_package_size;
    const size_t c_package_size;

    SDL_mutex *s_response_mutex;
    SDL_mutex *c_response_mutex;

    SDL_mutex *c_package_mutex;
    SDL_mutex *s_package_mutex;

    Log *log;
    SDL_mutex *log_mutex;
};