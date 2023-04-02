#define SDL_MAIN_HANDLED
#include "app.hpp"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif


// Main code
int main(int, char**)
{
    App *app = new App();
    delete app;
    return 0;
}
