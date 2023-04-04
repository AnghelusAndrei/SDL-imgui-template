#define SDL_MAIN_HANDLED
#include "app.hpp"

int main(){
    App app = App();

    while(app.Frame()){}
}