#include "client.hpp"
#include "SDL_net.h"


void DebugError(thread_data *data, char *error){
    data->WaitForMutex(data->debug_mutex);
    data->debug->debug = error;
    data->debug->debug_init = true;
    data->debug->debug_error = true;
    SDL_mutexV(data->debug_mutex);
}

void DebugMessage(thread_data *data, char *message){
    data->WaitForMutex(data->debug_mutex);
    data->debug->debug = message;
    data->debug->debug_init = true;
    data->debug->debug_error = false;
    SDL_mutexV(data->debug_mutex);
}

static int send_thread(void *data){
    thread_data *threadData = static_cast<thread_data*>(data);

    auto check_if_thread_active = [&]() 
    { 
        threadData->WaitForMutex(threadData->client_active_mutex);
        bool return_val = threadData->client_active;
        SDL_mutexV(threadData->client_active_mutex);
        return return_val;
    };


    while (check_if_thread_active())
    {
        if(!threadData->data_sent){
            threadData->WaitForMutex(threadData->client_data_mutex);
            if (SDLNet_TCP_Send(threadData->socket, (void *)threadData->client_data->buffer, SENT_BUFFER_SIZE) < SENT_BUFFER_SIZE) {
                DebugError(threadData, (char*)SDLNet_GetError());
            }
            threadData->data_sent = true;
            SDL_mutexV(threadData->client_data_mutex);
        }

    }
    return 0;
}

static int recieve_thread(void *data){
    thread_data *threadData = static_cast<thread_data*>(data);

    auto check_if_thread_active = [&]() 
    { 
        threadData->WaitForMutex(threadData->client_active_mutex);
        bool return_val = threadData->client_active;
        SDL_mutexV(threadData->client_active_mutex);
        return return_val;
    };


    while (check_if_thread_active())
    {

        if(!threadData->data_recieved){
            threadData->WaitForMutex(threadData->server_data_mutex);
            if (SDLNet_TCP_Recv(threadData->socket, threadData->server_data->buffer, RECIEVED_BUFFER_SIZE)){
                threadData->server_data->users = threadData->server_data->buffer[0];
            }
            threadData->data_recieved = true;
            SDL_mutexV(threadData->server_data_mutex);
        }

    }
    return 0;
}


Client::Client(){
    recieved *recieved_data = new recieved();
    sent *sent_data = new sent((char*)std::string("unnamed").c_str());
    debug_data *debug = new debug_data();

    data = new thread_data(sent_data, recieved_data, debug);

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

    data->WaitForMutex(data->client_active_mutex);
    data->client_active = true;
    SDL_mutexV(data->client_active_mutex);
    return 1;
}

bool Client::Disconnect(){
    if(data->client_active_mutex){
        data->WaitForMutex(data->client_active_mutex);
        data->client_active = false;
        SDL_mutexV(data->client_active_mutex);

        int recieveStatus;
        int sendStatus;
        SDL_WaitThread(recieveThread, &recieveStatus);
        SDL_WaitThread(sendThread, &sendStatus);
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

bool Client::InitRecieveingThread(){
    recieveThread = SDL_CreateThread(recieve_thread, "recieve_thread", data);
    if(recieveThread)return true;
    return false;
}

bool Client::InitSendingThread(){
    sendThread = SDL_CreateThread(send_thread, "send_thread", data);
    if(sendThread)return true;
    return false;
}

recieved *Client::Recieve(){

    if(data->data_recieved){
        data->WaitForMutex(data->server_data_mutex);
        memcpy(&main_recieved_data, data->server_data, sizeof(*data->server_data));
        data->data_recieved = false;
        SDL_mutexV(data->server_data_mutex);
    }


    return &main_recieved_data;
}

void Client::Send(sent *cpy){
    data->WaitForMutex(data->client_data_mutex);
    memcpy(data->client_data, cpy, sizeof(*cpy));
    data->data_sent = false;
    SDL_mutexV(data->client_data_mutex);
}

debug_data Client::GetDebug(){
    debug_data cpy;

    memcpy(&cpy, data->debug, sizeof(*data->debug));

    return cpy;
}   

IPaddress Client::GetIP(){
    IPaddress cpy;

    data->WaitForMutex(data->ip_mutex);
    cpy.host = data->server_address.host;
    cpy.port = data->server_address.port;
    SDL_mutexV(data->ip_mutex);

    return cpy;
}