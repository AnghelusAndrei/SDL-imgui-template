#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.hpp"
#include "client.hpp"

class App{
public:

    App(Client *client_);
    ~App();
    bool Run();

    Client *client;
    Interface *interface;
};