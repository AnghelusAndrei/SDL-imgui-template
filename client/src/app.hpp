#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgui/internal/imgui.h"
#include "imgui/internal/imgui_impl_sdl2.h"
#include "imgui/internal/imgui_impl_sdlrenderer.h"
#include "SDL.h"
#include "client.hpp"


class App{
    public:
    App(Client *client_);
    ~App();
    bool Frame();

    private:
    Client *client;

    SDL_Window *window;
    SDL_Renderer *renderer;
    ImGuiIO io;

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImVec2 GetWindowSize();
};