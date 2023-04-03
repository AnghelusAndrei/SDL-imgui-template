#define SDL_MAIN_HANDLED
#include "app.hpp"

int main(){
    Client *client = new Client(2000, (char*)std::string("192.168.1.6").c_str());
    App *app = new App(client);

    while(app->Frame()){}

    delete app;
    delete client;
}