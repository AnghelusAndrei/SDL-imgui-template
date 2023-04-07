#include <bits/stdc++.h>
#define MAX_SOCKETS 30

#define RECIEVED_BUFFER_SIZE 512
#define SENT_BUFFER_SIZE 512

#define GLOBAL_DATA 1
#define REQUESTED_DATA 2

struct client_data{
    uint16_t port;
    uint32_t host;

    char buffer[SENT_BUFFER_SIZE];
};

//this data is recieved from the server
struct recieved{
    char buffer[RECIEVED_BUFFER_SIZE] = "";
    uint8_t users = 0;
    std::unordered_map<uint64_t, client_data> clients;
};

//this data is sent to the server
struct sent{
    sent(char *c){
        std::strcpy(buffer, c);
    }
    char buffer[SENT_BUFFER_SIZE] = "";
};

struct debug_data{
    //data that needs to be passed to the window 
    char *debug;
    bool debug_init = false;
    bool debug_error = false;
};