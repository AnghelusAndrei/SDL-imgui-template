#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgui/internal/imgui.h"
#include "imgui/internal/imgui_impl_sdl2.h"
#include "imgui/internal/imgui_impl_sdlrenderer.h"
#include "client.hpp"


class Interface{
    public:
    Interface(Client *client_);
    ~Interface();
    bool frame();

    private:

    SDL_Window *window;
    SDL_Renderer *renderer;
    ImGuiIO io;

    Client *client;
    server_package *server;

    client_package data;

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    char input_name[16] = "";
    char input_text[64] = "";

    ImVec2 GetWindowSize();
};