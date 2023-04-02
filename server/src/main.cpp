#define SDL_MAIN_HANDLED
#include "window.hpp"

int main(){
    Server server = Server();
    Window window = Window();

    while(window.Frame(server.Run())){}
}