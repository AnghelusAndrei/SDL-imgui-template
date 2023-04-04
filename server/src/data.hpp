#include <bits/stdc++.h>
#define MAX_SOCKETS 30
#define SERVER_DELAY 300
#define BUFFER_SIZE 512

#define GLOBAL_DATA 1
#define REQUESTED_DATA 2

typedef struct {
    char name[BUFFER_SIZE];
    uint16_t port;
    uint32_t host;
} Client;

struct data{
    std::unordered_map<uint64_t, Client> clients;
};