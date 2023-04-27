#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.hpp"

class Server{
    public:
        Server(const uint16_t &port_);
        ~Server();
        bool Listen();

        uint64_t Hash_Address(uint32_t host, uint16_t port);

        void SendPackage(uint64_t client_key, server_package *package);
        std::pair<client_package*, bool> GetPackage(uint64_t client_key);
        Log GetLog();

        //server data
        IPaddress ip; 
        uint16_t port;
        uint16_t num_ready = 0;

        std::unordered_map<uint64_t, Client> clients;

        const size_t c_package_size;
        const size_t s_package_size;

    private:
        Log *log;
        SDL_mutex *log_mutex;

        std::unordered_map<uint64_t, SDL_Thread*> s_Thread;
        std::unordered_map<uint64_t, SDL_Thread*> c_Thread;

        TCPsocket server;
        SDLNet_SocketSet socket_set;

        std::unordered_map<uint64_t, client_package> last_package_recieved;
};