#include "client.hpp"
#include "SDL_net.h"

Client::Client(uint16_t port, char *ip){
    ServerData = new recieved();
    ClientData = new sent();
    if (SDLNet_Init() == -1) {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }

    if(SDLNet_ResolveHost(&server_address, ip, port) < 0) { 
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }

    socket = SDLNet_TCP_Open(&server_address);
    if (!socket)
    {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }
}

Client::~Client(){
    SDLNet_TCP_Close(socket);
    SDLNet_Quit();
}

recieved *Client::Recieve(){
    if (SDLNet_SocketReady(socket)) {
        char buffer[BUFFER_SIZE];
        int bytesReceived = SDLNet_TCP_Recv(socket, buffer, BUFFER_SIZE);
        if (bytesReceived <= 0) {
            // Server connection intrerrupted
            debug = (char*)std::string("Server connection intrerrupted").c_str();
            debug_init = true;
            debug_error = true;
            SDLNet_TCP_Close(socket);
            return ServerData;
        }

        std::strcpy(ServerData->buffer, buffer);
        ServerData->users = buffer[0];
    }
    return ServerData;
}

void Client::Send(sent *data){
    if (SDLNet_TCP_Send(socket, (void *)data->name, BUFFER_SIZE) < BUFFER_SIZE) {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }
}