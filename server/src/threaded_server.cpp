#include "threaded_server.hpp"
#define MAX_SOCKETS 30

void LOG(Log *log, SDL_mutex *mutex, const char *string){
    SDL_mutexP(mutex);
    log->AddLog(string);
    SDL_mutexV(mutex);
}

Server::Server(const uint16_t &port_) : port(port_), s_package_size(sizeof(server_package)), c_package_size(sizeof(client_package)), log(new Log()){
    log_mutex = SDL_CreateMutex();

    if (SDLNet_Init() < 0) { 
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    }

    if(SDLNet_ResolveHost(&ip, NULL, port) < 0) { 
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    }


    server = SDLNet_TCP_Open(&ip);
    if (!server)
    {
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    }

    socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1);
    if(socket_set == NULL) {
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    }
    
    if(SDLNet_TCP_AddSocket(socket_set, server) == -1) {
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    }
}

Server::~Server(){
    if(SDLNet_TCP_DelSocket(socket_set, server) == -1) {
        LOG(log, log_mutex, (char*)SDLNet_GetError());
    } SDLNet_TCP_Close(server);
    
    for (auto e : clients) {
        Client *c = &e.second;

        if(c->connection){
            c->connection = false;

            int s_Status;
            int c_Status;
            SDL_WaitThread(s_Thread[e.first], &s_Status);
            SDL_WaitThread(c_Thread[e.first], &c_Status);
        }
        
        if (c->socket != NULL) {
            
            SDLNet_TCP_DelSocket(socket_set,  c->socket);
            SDLNet_TCP_Close(c->socket);
            clients.erase(e.first);
        }
    }
    
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}


uint64_t Server::Hash_Address(uint32_t host, uint16_t port){
    return (uint64_t)(host << 16 | port);
}

static int c_thread(void *data){
    Client *c = static_cast<Client*>(data);

    c->c_package = new client_package();
    std::strcpy(c->c_package->name, std::string("unnamed").c_str());
    std::strcpy(c->c_package->text, std::string("").c_str());

    while (c->connection)
    {
        if(!c->c_package_action){
            SDL_mutexP(c->c_package_mutex);
            char buffer[c->c_package_size] = "";

            int bytesReceived = SDLNet_TCP_Recv( c->socket, buffer, c->c_package_size);
            if (bytesReceived <= 0) {
                // Client disconnected

                LOG(c->log, c->log_mutex, (char*)std::string("client disconnected").c_str());

                SDLNet_TCP_DelSocket(*c->socket_set,  c->socket);
                SDLNet_TCP_Close(c->socket);

                delete c;
                continue;
            }

            memcpy(&c->c_package, buffer, sizeof(buffer));

            c->c_package_action = true;
            SDL_mutexV(c->c_package_mutex);
        }

    }

    SDL_mutexP(c->c_response_mutex);
    c->c_response = true;
    SDL_mutexV(c->c_response_mutex);

    return 0;
}

static int s_thread(void *data){
    Client *c = static_cast<Client*>(data);
    
    c->s_package = new server_package();
    std::strcpy(c->s_package->text, std::string("").c_str());

    while (c->connection)
    {

        if(!c->s_package_action){
            SDL_mutexP(c->s_package_mutex);
            char buffer[c->s_package_size] = "";
            memcpy(buffer, c->s_package, sizeof(*c->s_package));
            if (SDLNet_TCP_Send(c->socket, (void *)buffer, c->s_package_size) < c->s_package_size) {
                LOG(c->log, c->log_mutex, (char*)SDLNet_GetError());
            }
            c->s_package_action = true;
            SDL_mutexV(c->s_package_mutex);
        }

    }

    SDL_mutexP(c->s_response_mutex);
    c->s_response = true;
    SDL_mutexV(c->s_response_mutex);

    return 0;
}


bool Server::Listen(){


    num_ready = SDLNet_CheckSockets(socket_set, 0);
    if(num_ready <= 0) {
        // NOTE: none of the sockets are ready

        return true;
    }

    //check for a new client connection
    if(SDLNet_SocketReady(server)) {
        if(clients.size() >= MAX_SOCKETS){
            LOG(log, log_mutex, (char*)std::string("Server full").c_str());
        }else{
            Client client(log, SDLNet_TCP_Accept(server));

            if (client.socket) {
                //get unique id
                IPaddress *address = SDLNet_TCP_GetPeerAddress(client.socket);
                uint64_t id = Hash_Address(address->host, address->port);

                clients.insert(std::make_pair(id, client));

                // Add the client socket to the socket set
                if (SDLNet_TCP_AddSocket(socket_set, client.socket) == -1) {
                    LOG(log, log_mutex, (char*)SDLNet_GetError());
                    SDLNet_TCP_Close(client.socket);
                    clients.erase(id);
                    return false;
                }

                LOG(log, log_mutex, (char*)std::string("New client connected").c_str());

                clients[id].socket_set = &socket_set;
                clients[id].host = address->host;
                clients[id].port = address->port;
                clients[id].log_mutex = log_mutex;

                client_package cp;
                last_package_recieved.insert(std::make_pair(id, cp));

                s_Thread.insert(std::make_pair(id, SDL_CreateThread(s_thread, (char*)std::string("s_thread" + std::to_string(id)).c_str(), &clients[id])));
                c_Thread.insert(std::make_pair(id, SDL_CreateThread(c_thread, (char*)std::string("c_thread" + std::to_string(id)).c_str(), &clients[id])));

            }
        }
    }
    
    
    return true;
}


void Server::SendPackage(uint64_t client_key, server_package *package){
    SDL_mutexP(clients[client_key].c_package_mutex);
    memcpy(clients[client_key].c_package, package, sizeof(*package));
    clients[client_key].c_package_action = false;
    SDL_mutexV(clients[client_key].c_package_mutex);
}

std::pair<client_package*, bool> Server::GetPackage(uint64_t client_key){

    if(clients[client_key].s_package_action){
        SDL_mutexP(clients[client_key].s_package_mutex);
        memcpy(&last_package_recieved[client_key], clients[client_key].s_package, sizeof(*clients[client_key].s_package));
        clients[client_key].s_package_action = false;
        SDL_mutexV(clients[client_key].s_package_mutex);

        return std::make_pair(&last_package_recieved[client_key], true);
    }


    return std::make_pair(&last_package_recieved[client_key], false);
}

Log Server::GetLog(){
    return *log;
}