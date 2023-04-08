#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "SDL.h"
#include "SDL_net.h"
#include "data.hpp"

class thread_data{
public:
    thread_data(client_package *c_package_, server_package *s_package_, debug_data *debug_);
    ~thread_data();

    //data

    /*
    this is true while the client thread is running and is turned 
    false by the main thread in order to stop the client thread
    */
    bool client_active = false;

    /*
    the data being sent to the server
    */
    client_package *c_package;

    /*
    *this is used for communication between threads

    it shares a mutex with client_data

    it is turned false when data is sent to the server
    and true when new data is waiting to be sent
    */
    bool c_package_bool = false;

    /*
    this is the data recieved from the server
    */
    server_package *s_package;

    /*
    *this is used for communication between threads

    it shares a mutex with server_data

    it is true after the server data is recieved by the client
    and false after the data is recieved from the client by the 
    main thread
    */
    bool s_package_bool = false;

    debug_data *debug;
    IPaddress server_address;

    //mutexes
    SDL_mutex *client_active_mutex;
    SDL_mutex *c_package_mutex;
    SDL_mutex *s_package_mutex;
    SDL_mutex *debug_mutex;
    SDL_mutex *ip_mutex;

    //package size
    size_t s_package_size;
    size_t c_package_size;

    //this function is blocking
    bool WaitForMutex(SDL_mutex *mutex);

    uint64_t Hash_Address(uint32_t host, uint16_t port);
    TCPsocket socket;
};