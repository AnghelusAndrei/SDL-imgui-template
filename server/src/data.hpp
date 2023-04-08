#include <bits/stdc++.h>

#define MAX_SOCKETS 30
#define SERVER_DELAY 10

//--packages

typedef struct{
    char name[16];
    char text[64];
    bool request;
} client_package;


typedef struct{
    uint8_t num_users;
    char text[4][64];
} server_package;

//--

typedef struct {
    client_package *package;
    uint16_t port;
    uint32_t host;
} Client;