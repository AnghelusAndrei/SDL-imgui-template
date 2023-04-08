#pragma once
#include "client_thread.hpp"

class Client{
    public:
        Client();
        ~Client();
        
        bool Connect(uint16_t port, char *ip);
        bool Disconnect();
        bool InitC_Thread();
        bool InitS_Thread();

        server_package *s_package();
        void Send(client_package *cpy);
        debug_data GetDebug();
        IPaddress GetIP();
        
        //this is where every client-related data is stored
        thread_data *data;

        server_package s_data;

    private:
        SDL_Thread *s_Thread;
        SDL_Thread *c_Thread;
        bool connected = false;
};