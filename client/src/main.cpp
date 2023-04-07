#define SDL_MAIN_HANDLED
#include "app.hpp"

int main(){
    Client *client = new Client();
    App *app = new App(client);

    app->Run();

    delete app;
}