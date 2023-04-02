#pragma once
#include "imgui/internal/imgui.h"
#include "imgui/internal/imgui_impl_sdl2.h"
#include "imgui/internal/imgui_impl_sdlrenderer.h"
#include <stdio.h>
#include <bits/stdc++.h>
#include "SDL.h"

#define LOGIN 1
#define HOME 2
#define REQUEST 3
#define INBOX 4

class App{
    public:
    App();
    ~App();


    private:
    int stage;
    SDL_Renderer* renderer;
    SDL_Window* window;

    void Login();
    void Request();
    void Inbox();
    void Home();

    void TextCentered(std::string text);
    void SetWidgetStyle();
};