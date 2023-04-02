#include "server.hpp"

Server::Server(){
    ServerData.error = "0";

    if (SDLNet_Init() < 0) { 
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }

    if(SDLNet_ResolveHost(&ip, NULL, 2000) < 0) { 
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }

    /* Open a connection with the IP provided (listen on the host's port) */ 
    if (!(sd = SDLNet_TCP_Open(&ip))) { 
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }
}

Server::~Server(){
    SDLNet_TCP_Close(sd);

    SDLNet_Quit();
}

data Server::Run(){
    data ServerData;
    if ((csd = SDLNet_TCP_Accept(sd))) {
        if ((remoteIP = SDLNet_TCP_GetPeerAddress(csd))){
            ServerData.host = SDLNet_Read32(&remoteIP->host);
            ServerData.port = SDLNet_Read16(&remoteIP->port); 
        }else{
            ServerData.error = (char*)SDLNet_GetError();
            ServerData.stopped = true;
        }
    }
    return ServerData;
}