#include "client_thread.hpp"

thread_data::thread_data(sent *client_data_, recieved *server_data_, debug_data *debug_) :
client_data(client_data_), server_data(server_data_), debug(debug_)
{
    client_active_mutex = SDL_CreateMutex();
    client_data_mutex = SDL_CreateMutex();
    server_data_mutex = SDL_CreateMutex();
    debug_mutex = SDL_CreateMutex();
    ip_mutex = SDL_CreateMutex();
}

thread_data::~thread_data(){
    delete client_data;
    delete debug;
    delete server_data;

    SDL_DestroyMutex(client_active_mutex);
    SDL_DestroyMutex(client_data_mutex);
    SDL_DestroyMutex(server_data_mutex);
    SDL_DestroyMutex(debug_mutex);
    SDL_DestroyMutex(ip_mutex);
}

bool thread_data::WaitForMutex(SDL_mutex *mutex){
    while(SDL_mutexP(mutex) == -1){}
    return true;
}