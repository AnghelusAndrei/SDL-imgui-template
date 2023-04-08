#include <bits/stdc++.h>

#define MAX_SOCKETS 30

//--packages

typedef struct{
    char name[16];
    char text[64];
    bool request;
} client_package;


typedef struct{
    uint8_t num_users;
    char text1[64];
    char text2[64];
    char text3[64];
    char text4[64];
} server_package;

//--


struct debug_data{
    //data that needs to be passed to the window 
    char *debug;
    bool debug_init = false;
    bool debug_error = false;
};