#include "app.hpp"

App::App(Client *client_) : client(client_)
{
    client->Connect(2000, (char*)std::string("192.168.1.6").c_str());
    client->InitSendingThread();
    client->InitRecieveingThread();
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