#include "app.hpp"

App::App(Client *client_) : client(client_)
{
    client->Connect(2000, (char*)std::string("localhost").c_str());
    client->InitC_Thread();
    client->InitS_Thread();
    interface = new Interface(client);
}

App::~App(){
    delete interface;
    delete client;
}

bool App::Run(){
    while(interface->frame()){}
    client->Disconnect();
    return true;
}