#include "client.hpp"


void DebugError(thread_data *data, char *error){
    SDL_mutexP(data->debug_mutex);
    data->debug->debug = error;
    data->debug->debug_init = true;
    data->debug->debug_error = true;
    SDL_mutexV(data->debug_mutex);
}

void DebugMessage(thread_data *data, char *message){
    SDL_mutexP(data->debug_mutex);
    data->debug->debug = message;
    data->debug->debug_init = true;
    data->debug->debug_error = false;
    SDL_mutexV(data->debug_mutex);
}

static int c_thread(void *data){
    thread_data *threadData = static_cast<thread_data*>(data);

    while (threadData->client_active)
    {
        if(!threadData->c_package_bool){
            SDL_mutexP(threadData->c_package_mutex);
            char buffer[threadData->c_package_size] = "";
            memcpy(buffer, threadData->c_package, sizeof(*threadData->c_package));
            if (SDLNet_TCP_Send(threadData->socket, (void *)buffer, threadData->c_package_size) < threadData->c_package_size) {
                DebugError(threadData, (char*)SDLNet_GetError());
            }
            threadData->c_package_bool = true;
            SDL_mutexV(threadData->c_package_mutex);
        }

    }
    return 0;
}

static int s_thread(void *data){
    thread_data *threadData = static_cast<thread_data*>(data);

    while (threadData->client_active)
    {

        if(!threadData->s_package_bool){
            SDL_mutexP(threadData->s_package_mutex);
            char buffer[threadData->s_package_size] = "";
            if (SDLNet_TCP_Recv(threadData->socket, buffer, threadData->s_package_size)){
                memcpy(threadData->s_package, buffer, sizeof(buffer));
            }
            threadData->s_package_bool = true;
            SDL_mutexV(threadData->s_package_mutex);
        }

    }
    return 0;
}


Client::Client(){
    server_package *s_package = new server_package();
    client_package *c_package = new client_package();
    debug_data *debug = new debug_data();

    strcpy(c_package->name, (char*)std::string("unnamed").c_str());

    data = new thread_data(c_package, s_package, debug);

    if (SDLNet_Init() == -1) {
        DebugError(data, (char*)SDLNet_GetError());
    }
}

bool Client::Connect(uint16_t port, char *ip){

    if(SDLNet_ResolveHost(&data->server_address, ip, port) < 0) { 
        DebugError(data,(char*)SDLNet_GetError());
        return 0;
    }

    data->socket = SDLNet_TCP_Open(&data->server_address);
    if (!data->socket)
    {
        DebugError(data,(char*)SDLNet_GetError());
        return 0;
    }

    connected = true;

    SDL_mutexP(data->client_active_mutex);
    data->client_active = true;
    SDL_mutexV(data->client_active_mutex);
    return 1;
}

bool Client::Disconnect(){
    if(data->client_active){
        SDL_mutexP(data->client_active_mutex);
        data->client_active = false;
        SDL_mutexV(data->client_active_mutex);

        int s_Status;
        int c_Status;
        SDL_WaitThread(s_Thread, &s_Status);
        SDL_WaitThread(c_Thread, &c_Status);
    }

    if(connected){
        SDLNet_TCP_Close(data->socket);
        return 1;
        connected = false;
    }
    return 0;
}

Client::~Client(){
    Disconnect();
    SDLNet_Quit();
}

bool Client::InitS_Thread(){
    s_Thread = SDL_CreateThread(s_thread, "s_thread", data);
    if(s_Thread){
        Send(data->c_package);
        return true;
    }
    return false;
}

bool Client::InitC_Thread(){
    c_Thread = SDL_CreateThread(c_thread, "c_thread", data);
    if(c_Thread)return true;
    return false;
}

server_package *Client::s_package(){

    if(data->s_package_bool){
        SDL_mutexP(data->s_package_mutex);
        memcpy(&s_data, data->s_package, sizeof(*data->s_package));
        data->s_package_bool = false;
        SDL_mutexV(data->s_package_mutex);
    }


    return &s_data;
}

void Client::Send(client_package *cpy){
    SDL_mutexP(data->c_package_mutex);
    memcpy(data->c_package, cpy, sizeof(*cpy));
    data->c_package_bool = false;
    SDL_mutexV(data->c_package_mutex);
}

debug_data Client::GetDebug(){
    debug_data cpy;

    memcpy(&cpy, data->debug, sizeof(*data->debug));

    return cpy;
}   

IPaddress Client::GetIP(){
    IPaddress cpy;

    SDL_mutexP(data->ip_mutex);
    cpy.host = data->server_address.host;
    cpy.port = data->server_address.port;
    SDL_mutexV(data->ip_mutex);

    return cpy;
}