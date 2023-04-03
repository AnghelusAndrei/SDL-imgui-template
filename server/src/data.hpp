#include <bits/stdc++.h>
#define MAX_SOCKETS 10
#define SERVER_DELAY 10

typedef struct {
    int in_use;
    int questing;
} Client;

struct data{
    bool quit = false;
    bool stopped = false;
    int num_ready = 0;
    int next_ind = 0;
    char *error;
    Client clients[MAX_SOCKETS];
};