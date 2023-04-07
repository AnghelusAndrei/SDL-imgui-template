#include "server.hpp"
#include "SDL_net.h"

Server::Server(const uint16_t &port_) : port(port_){
    ServerData = new data();

    if (SDLNet_Init() < 0) { 
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }

    if(SDLNet_ResolveHost(&ip, NULL, port) < 0) { 
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }


    server = SDLNet_TCP_Open(&ip);
    if (!server)
    {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }

    socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1);
    if(socket_set == NULL) {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }
    
    if(SDLNet_TCP_AddSocket(socket_set, server) == -1) {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
    }
}

Server::~Server(){
    if(SDLNet_TCP_DelSocket(socket_set, server) == -1) {
        debug = (char*)SDLNet_GetError();
        debug_init = true;
        debug_error = true;
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

    num_ready = SDLNet_CheckSockets(socket_set, 0);
    if(num_ready <= 0) {
        // NOTE: none of the sockets are ready

        // Send data to all connected users
        char raw_send_data[BUFFER_SIZE] = "";
        uint8_t num_users = (uint8_t)(ServerData->clients.size());
        raw_send_data[0] = (char)num_users;
        raw_send_data[1] = (char)(SDL_GetTicks()%256);

        for (auto e : sockets) {
            if (SDLNet_TCP_Send(e.second, (void *)raw_send_data, BUFFER_SIZE) < BUFFER_SIZE) {
                debug = (char*)SDLNet_GetError();
                debug_init = true;
                debug_error = true;
            }
        }
        return ServerData;
    }

    //check for a new client connection
    if(SDLNet_SocketReady(server)) {
        if(sockets.size() > MAX_SOCKETS){
            debug = (char*)std::string("Server full").c_str();
            debug_init = true;
        }else{
            TCPsocket client_socket = SDLNet_TCP_Accept(server);

            if (client_socket) {
                //get unique id
                IPaddress *address = SDLNet_TCP_GetPeerAddress(client_socket);
                uint64_t id = Hash_Address(address->host, address->port);
                sockets.insert(std::make_pair(id, client_socket));

                // Add the client socket to the socket set
                if (SDLNet_TCP_AddSocket(socket_set, sockets[id]) == -1) {
                    debug = (char*)SDLNet_GetError();
                    debug_init = true;
                    debug_error = true;
                    SDLNet_TCP_Close(sockets[id]);
                } else {
                    debug = (char*)std::string("New client connected").c_str();
                    debug_init = true;
                    
                    Client client;
                    std::strcpy(client.name, (char*)std::string("unnamed").c_str());
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
            int bytesReceived = SDLNet_TCP_Recv( client_socket, buffer, BUFFER_SIZE);
            if (bytesReceived <= 0) {
                // Client disconnected
                debug = (char*)std::string("client disconnected").c_str();
                debug_init = true;
                SDLNet_TCP_DelSocket(socket_set,  client_socket);
                SDLNet_TCP_Close(client_socket);
                sockets.erase(e.first);

                ServerData->clients.erase(e.first);
                continue;
            }else if(bytesReceived == BUFFER_SIZE){
                std::strcpy(ServerData->clients[e.first].name, buffer);
            }

        }
    }
    
    
    return ServerData;
}