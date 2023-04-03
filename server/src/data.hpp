#include <bits/stdc++.h>
#define MAX_SOCKETS 30
#define SERVER_DELAY 300
#define BUFFER_SIZE 512

typedef struct {
    uint8_t in_use;
    uint16_t port;
    uint32_t host;
} Client;

struct data{
    //data that needs to be passed to the window 
    char *debug;
    bool debug_init = false;
    bool debug_error = false;

    //server data
    IPaddress ip; 
    uint16_t port = 2000;
    uint16_t num_ready = 0;
    std::unordered_map<uint64_t, Client> clients;
};