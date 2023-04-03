#include "server.hpp"
#include "SDL_net.h"

Server::Server(const uint16_t &port){
    ServerData = new data();

    if (SDLNet_Init() < 0) { 
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }

    if(SDLNet_ResolveHost(&ServerData->ip, NULL, ServerData->port) < 0) { 
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }


    server = SDLNet_TCP_Open(&ServerData->ip);
    if (!server)
    {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }

    socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1);
    if(socket_set == NULL) {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }
    
    if(SDLNet_TCP_AddSocket(socket_set, server) == -1) {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    }
}

Server::~Server(){
    if(SDLNet_TCP_DelSocket(socket_set, server) == -1) {
        ServerData->debug = (char*)SDLNet_GetError();
        ServerData->debug_init = true;
        ServerData->debug_error = true;
    } SDLNet_TCP_Close(server);
    
    for (auto e : sockets) {
        TCPsocket client_socket = e.second;
        if (client_socket != NULL) {
            SDLNet_TCP_DelSocket(socket_set,  client_socket);
            SDLNet_TCP_Close(client_socket);
            sockets.erase(e.first);
            ServerData->clients.erase(e.first);
        }
    }
    
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}


uint64_t Server::Hash_Address(uint32_t host, uint16_t port){
    return (uint64_t)(host << 16 | port);
}


data *Server::Run(){
    if(time == 0){
        time = SDL_GetTicks();
        return ServerData;
    }
    if(SDL_GetTicks() < time + SERVER_DELAY){
        return ServerData;
    }

    time = SDL_GetTicks();

    ServerData->num_ready = SDLNet_CheckSockets(socket_set, 0);
    if(ServerData->num_ready <= 0) {
        // NOTE: none of the sockets are ready
        return ServerData;
    }

    //check for a new client connection
    if(SDLNet_SocketReady(server)) {
        if(sockets.size() > MAX_SOCKETS){
            ServerData->debug = (char*)std::string("Server full").c_str();
            ServerData->debug_init = true;
        }else{
            TCPsocket client_socket = SDLNet_TCP_Accept(server);

            if (client_socket) {
                //get unique id
                IPaddress *address = SDLNet_TCP_GetPeerAddress(client_socket);
                uint64_t id = Hash_Address(address->host, address->port);
                sockets.insert(std::make_pair(id, client_socket));

                // Add the client socket to the socket set
                if (SDLNet_TCP_AddSocket(socket_set, sockets[id]) == -1) {
                    ServerData->debug = (char*)SDLNet_GetError();
                    ServerData->debug_init = true;
                    ServerData->debug_error = true;
                    SDLNet_TCP_Close(sockets[id]);
                } else {
                    ServerData->debug = (char*)std::string("New client connected").c_str();
                    ServerData->debug_init = true;
                    
                    Client client;
                    client.in_use = 1;
                    client.host = address->host;
                    client.port = address->port;
                    ServerData->clients.insert(std::make_pair(id, client));
                }
            }
        }
    }

    // Check for activity on client sockets
    for (auto e : sockets) {
        TCPsocket client_socket = e.second;
        if (SDLNet_SocketReady( client_socket)) {
            char buffer[BUFFER_SIZE];
            int bytesReceived = SDLNet_TCP_Recv( client_socket, buffer, BUFFER_SIZE - 1);
            if (bytesReceived <= 0) {
                // Client disconnected
                ServerData->debug = (char*)std::string("client disconnected").c_str();
                ServerData->debug_init = true;
                SDLNet_TCP_DelSocket(socket_set,  client_socket);
                SDLNet_TCP_Close(client_socket);
                sockets.erase(e.first);

                ServerData->clients.erase(e.first);
                break;
            }
        }
    }
    
    
    return ServerData;
}