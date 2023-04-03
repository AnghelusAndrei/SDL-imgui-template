#include "server.hpp"
#include "SDL_net.h"

Server::Server(){

    if (SDLNet_Init() < 0) { 
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }

    if(SDLNet_ResolveHost(&ip, NULL, 2000) < 0) { 
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }


    server = SDLNet_TCP_Open(&ip);
    if (!server)
    {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }

    socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1);
    if(socket_set == NULL) {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }
    
    if(SDLNet_TCP_AddSocket(socket_set, server) == -1) {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }
}

void Server::CloseSocket(int index) {
    if(sockets[index] == NULL) {
        ServerData.error = (char*)std::string("Attempted to delete a NULL socket").c_str();
        ServerData.stopped = true;
    }
 
    if(SDLNet_TCP_DelSocket(socket_set, sockets[index]) == -1) {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }
 
    memset(&ServerData.clients[index], 0x00, sizeof(Client));
    SDLNet_TCP_Close(sockets[index]);
    sockets[index] = NULL;
}

Server::~Server(){
    if(SDLNet_TCP_DelSocket(socket_set, server) == -1) {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    } SDLNet_TCP_Close(server);
    
    int i;
    for(i=0; i<MAX_SOCKETS; ++i) {
        if(sockets[i] == NULL) continue;
        CloseSocket(i);
    }
    
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}

int Server::AcceptSocket(int index) {
    if(sockets[index]) {
        CloseSocket(index);
    }
 
    sockets[index] = SDLNet_TCP_Accept(server);
    if(sockets[index] == NULL) return 0;
 
    ServerData.clients[index].in_use = 1;
    if(SDLNet_TCP_AddSocket(socket_set, sockets[index]) == -1) {
        ServerData.error = (char*)SDLNet_GetError();
        ServerData.stopped = true;
    }
 
    return 1;
}

data Server::Run(){
    if(time == 0){
        time = SDL_GetTicks();
        return ServerData;
    }
    if(SDL_GetTicks() < time + SERVER_DELAY){
        time = SDL_GetTicks();
        return ServerData;
    }

    ServerData.num_ready = SDLNet_CheckSockets(socket_set, 0);
    if(ServerData.num_ready <= 0) {
        // NOTE: none of the sockets are ready
    } else {
        // NOTE: some number of the sockets are ready
    }

    if(SDLNet_SocketReady(server)) {
        
        int got_socket = AcceptSocket(ServerData.next_ind);
        if(!got_socket) {
            ServerData.num_ready--;
            return ServerData;
        }
    
        // NOTE: get a new index
        int chk_count;
        for(chk_count=0; chk_count<MAX_SOCKETS; chk_count++) {
            if(sockets[(ServerData.next_ind+chk_count)%MAX_SOCKETS] == NULL) break;
        }
    
        ServerData.next_ind = (ServerData.next_ind+chk_count)%MAX_SOCKETS;
    
        ServerData.num_ready--;
    }
    return ServerData;
}