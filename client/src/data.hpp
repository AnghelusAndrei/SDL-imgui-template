#include <bits/stdc++.h>

//--packages

typedef struct{
    char name[16] = "";
    char text[64] = "";
} client_package;


typedef struct{
    uint8_t num_users = 0;
    char text[64] = "";
} server_package;

//--

struct debug_data{
    //data that needs to be passed to the window 
    char *debug;
    bool debug_init = false;
    bool debug_error = false;
};