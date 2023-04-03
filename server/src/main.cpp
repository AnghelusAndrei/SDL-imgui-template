#define SDL_MAIN_HANDLED
#include "app.hpp"

int main(){
    Server server = Server(2000);
    App app = App();

    while(app.Frame(server.Run())){}
}