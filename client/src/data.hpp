#include <bits/stdc++.h>
#define MAX_SOCKETS 30
#define SERVER_DELAY 300
#define BUFFER_SIZE 512

#define GLOBAL_DATA 1
#define REQUESTED_DATA 2

struct client_data{
    uint16_t port;
    uint32_t host;

    char name[BUFFER_SIZE];
};

struct recieved{
    char buffer[BUFFER_SIZE];
    uint8_t users = 0;
    std::unordered_map<uint64_t, client_data> clients;
};

struct sent{
    char name[BUFFER_SIZE] = "unnamed";
};