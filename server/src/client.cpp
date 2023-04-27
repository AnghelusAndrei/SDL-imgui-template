#include "client.hpp"

Client::Client() :
s_package_size(sizeof(server_package)), c_package_size(sizeof(client_package))
{
    c_package_mutex = SDL_CreateMutex();
    s_package_mutex = SDL_CreateMutex();

    s_response_mutex = SDL_CreateMutex();
    c_response_mutex = SDL_CreateMutex();
}

Client::Client(Log *log_, TCPsocket socket_) :
s_package_size(sizeof(server_package)), c_package_size(sizeof(client_package)), log(log_), socket(socket_)
{
    c_package_mutex = SDL_CreateMutex();
    s_package_mutex = SDL_CreateMutex();

    s_response_mutex = SDL_CreateMutex();
    c_response_mutex = SDL_CreateMutex();
}

Client::~Client(){
    connection = false;

    while(s_response || c_response){}

    delete c_package;
    delete s_package;

    SDL_DestroyMutex(c_package_mutex);
    SDL_DestroyMutex(s_package_mutex);
    
    SDL_DestroyMutex(s_response_mutex);
    SDL_DestroyMutex(c_response_mutex);
}
