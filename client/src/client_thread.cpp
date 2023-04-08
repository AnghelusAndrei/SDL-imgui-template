#include "client_thread.hpp"

thread_data::thread_data(client_package *c_package_, server_package *s_package_, debug_data *debug_) :
c_package(c_package_), s_package(s_package_), debug(debug_), s_package_size(sizeof(server_package)), c_package_size(sizeof(client_package))
{
    client_active_mutex = SDL_CreateMutex();
    c_package_mutex = SDL_CreateMutex();
    s_package_mutex = SDL_CreateMutex();
    debug_mutex = SDL_CreateMutex();
    ip_mutex = SDL_CreateMutex();
}

thread_data::~thread_data(){
    delete c_package;
    delete s_package;
    delete debug;

    SDL_DestroyMutex(client_active_mutex);
    SDL_DestroyMutex(c_package_mutex);
    SDL_DestroyMutex(s_package_mutex);
    SDL_DestroyMutex(debug_mutex);
    SDL_DestroyMutex(ip_mutex);
}

bool thread_data::WaitForMutex(SDL_mutex *mutex){
    while(SDL_mutexP(mutex) == -1){}
    return true;
}