#include <bits/stdc++.h>

struct client{
    int id;
};

struct data{
    bool quit = false;
    uint16_t port;
    uint32_t host;
    char *error;
    bool stopped = false;
    std::vector<client> clients;
};