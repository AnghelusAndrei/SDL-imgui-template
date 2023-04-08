#include <bits/stdc++.h>

#define MAX_SOCKETS 30
#define SERVER_DELAY 10

//--packages

typedef struct{
    char name[16] = "";
    char text[64] = "";
    bool request;
} client_package;


typedef struct{
    uint8_t num_users = 0;
    char text1[64] = "";
    char text2[64] = "";
    char text3[64] = "";
    char text4[64] = "";
} server_package;

//--

typedef struct {
    client_package package;
    uint16_t port;
    uint32_t host;
} Client;