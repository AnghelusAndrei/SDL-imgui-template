#include "client.hpp"
#include "SDL_net.h"

Client::Client(uint16_t port, char *ip){
    ServerData = new recieved();
    if (SDLNet_Init() == -1) {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }

    if(SDLNet_ResolveHost(&server_address, ip, port) < 0) { 
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }

    socket = SDLNet_TCP_Open(&server_address);
    if (!socket)
    {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }
}

Client::~Client(){
    // Close the TCPsocket object on the client side
    SDLNet_TCP_Close(socket);
}

recieved *Client::Recieve(){

    return ServerData;
}

void Send(sent *data){

}