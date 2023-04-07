#pragma once
#include "client_thread.hpp"

class Client{
    public:
        Client();
        ~Client();
        
        bool Connect(uint16_t port, char *ip);
        bool Disconnect();
        bool InitRecieveingThread();
        bool InitSendingThread();
        recieved *Recieve();
        void Send(sent *cpy);
        debug_data GetDebug();
        IPaddress GetIP();
        
        //this is where every client-related data is stored
        thread_data *data;

        recieved main_recieved_data;

    private:
        SDL_Thread *recieveThread;
        SDL_Thread *sendThread;
        bool connected = false;
};