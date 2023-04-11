#include <bits/stdc++.h>

#define MAX_SOCKETS 30

//--packages

typedef struct{
    char name[16] = "";
    char text[64] = "";
} client_package;


typedef struct{
    uint8_t num_users = 0;
    char text[10][64] = {"","","",""};
} server_package;

//--

typedef struct {
    client_package package;
    uint16_t port;
    uint32_t host;
} Client;