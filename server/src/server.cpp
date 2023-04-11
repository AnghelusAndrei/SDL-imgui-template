#include "server.hpp"
#include "SDL_net.h"

Server::Server(const uint16_t &port_) : port(port_), s_package_size(sizeof(server_package)), c_package_size(sizeof(client_package)){
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
            clients.erase(e.first);
        }
    }
    
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}


uint64_t Server::Hash_Address(uint32_t host, uint16_t port){
    return (uint64_t)(host << 16 | port);
}


bool Server::Listen(){

    // Send data to all connected users
    char raw_send_data[s_package_size] = "";
    s_package.num_users = (uint8_t)(clients.size());
    memcpy(raw_send_data, &s_package, sizeof(s_package));

    for (auto e : sockets) {
        if (SDLNet_TCP_Send(e.second, (void *)raw_send_data, s_package_size) < s_package_size) {
            debug = (char*)SDLNet_GetError();
            debug_init = true;
            debug_error = true;
        }
    }

    num_ready = SDLNet_CheckSockets(socket_set, 0);
    if(num_ready <= 0) {
        // NOTE: none of the sockets are ready

        return true;
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
                    client.host = address->host;
                    client.port = address->port;
                    clients.insert(std::make_pair(id, client));
                }
            }
        }
    }

    // Check for activity on client sockets
    for (auto e : sockets) {
        TCPsocket client_socket = e.second;
        if (SDLNet_SocketReady( client_socket)) {
            char buffer[c_package_size];
            int bytesReceived = SDLNet_TCP_Recv( client_socket, buffer, c_package_size);
            if (bytesReceived <= 0) {
                // Client disconnected
                debug = (char*)std::string("client disconnected").c_str();
                debug_init = true;

                for(int i = 0; i < sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1; i++){
                    std::strcpy(s_package.text[i], s_package.text[i+1]);
                }
                std::string txt = std::string("User ") + std::string(clients[e.first].package.name) + std::string(" left the room");

                std::strcpy(s_package.text[sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1], (char*)txt.c_str());  

                SDLNet_TCP_DelSocket(socket_set,  client_socket);
                SDLNet_TCP_Close(client_socket);
                sockets.erase(e.first);

                clients.erase(e.first);
                continue;
            }

            char text[64];
            char name[16];
            std::strcpy(text, clients[e.first].package.text);
            std::strcpy(name, clients[e.first].package.name);

            memcpy(&clients[e.first].package, buffer, sizeof(buffer));


            //impelentation specific
            if(std::strcmp(name, clients[e.first].package.name) == 0){
                for(int i = 0; i < sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1; i++){
                    std::strcpy(s_package.text[i], s_package.text[i+1]);
                }
                std::string txt = std::string(clients[e.first].package.name) + ": " + std::string(clients[e.first].package.text);

                std::strcpy(s_package.text[sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1], (char*)txt.c_str());
            }else{
                if(strlen(name) > 0){
                    for(int i = 0; i < sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1; i++){
                        std::strcpy(s_package.text[i], s_package.text[i+1]);
                    }
                    std::string txt = std::string("User ") + std::string(name) + std::string(" changed username to ") + std::string(clients[e.first].package.name);

                    std::strcpy(s_package.text[sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1], (char*)txt.c_str());
                }else{
                    for(int i = 0; i < sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1; i++){
                        std::strcpy(s_package.text[i], s_package.text[i+1]);
                    }
                    std::string txt = std::string("User ") + std::string(clients[e.first].package.name) + std::string(" joined the room");

                    std::strcpy(s_package.text[sizeof(s_package.text)/sizeof(clients[e.first].package.text) - 1], (char*)txt.c_str());   
                }

            }
        }
    }
    
    
    return true;
}