#define SDL_MAIN_HANDLED
#include "imgui/internal/imgui.h"
#include "imgui/internal/imgui_impl_sdl2.h"
#include "imgui/internal/imgui_impl_sdlrenderer.h"
#include <stdio.h>
#include "engine/sr_core.hpp"
#include "SDL.h"


#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

ImVec2 GetWindowSize(SDL_Window *window){
    std::pair<int, int> size;
    SDL_GetWindowSize(window, &size.first, &size.second);
    return ImVec2(size.first, size.second);
}

// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    uint32_t ms_f1;
    uint32_t ms_f2;

    sr::internal_buffer_object *buffers;
    sr::mesh Cube;
    sr::player_t Camera = {0,0,0,0,0};
    sr::vec3 light;

    ms_f1 = SDL_GetTicks();

    ImVec2 window_size = GetWindowSize(window);
    buffers = new sr::internal_buffer_object(sr::ivec2((int)window_size.x, (int)window_size.y));

    Cube.LoadFile("./assets/cow.obj");

    light.x = -1;
    light.y = -0.75;
    light.z = -0.5;

    Cube.position.x = 0;
    Cube.position.y = 0;
    Cube.position.z = 5;

    Cube.size.x = 0.5;
    Cube.size.y = 0.5;
    Cube.size.z = 0.5;

    Cube.color = {255, 100, 50};
    
    std::function<void(sr::ivec2 pixel, sr::color color)> setpixel = [renderer](sr::ivec2 pixel, sr::color color){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
    };

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window)){
                ImVec2 window_size = GetWindowSize(window);
                buffers->update_framebuffer_size(sr::ivec2((int)window_size.x, (int)window_size.y));
            }
        }

        ImVec2 window_size = GetWindowSize(window);

        float FOV = 60;
        float Zfar = 1000;
        float Znear = 0.1;
        sr::mat4x4 Projection_Matrix = sr::Matrix_Projection(sr::ivec2((int)window_size.x, (int)window_size.y), FOV, Zfar, Znear);


        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        {
            ImGuiWindowFlags flags = 0;
            flags |= ImGuiWindowFlags_NoResize;
            ImGui::Begin("Debug profile", NULL, flags);

            ms_f2 = SDL_GetTicks();

            ImGui::Text("frame ms: %i", ms_f2 - ms_f1);
            ImGui::Text("frame fps: %f", 1000.0f / (float)(ms_f2 - ms_f1));

            ms_f1 = ms_f2;

            ImGui::End();
        }

        
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);


        Cube.rotation.x = (float)(SDL_GetTicks()/50);
        Cube.rotation.y = (float)(SDL_GetTicks()/50);
        Cube.rotation.z = (float)(SDL_GetTicks()/50);

        int r1 = (SDL_GetTicks()/20)%410 + 50;
        int r2 = (SDL_GetTicks()/10)%410 + 50;
        int r3 = (SDL_GetTicks()/25)%410 + 50;

        Cube.color.r = r1 > 255 ? (510 - r1) : r1;
        Cube.color.g = r2 > 255 ? (510 - r2) : r2;
        Cube.color.b = r3 > 255 ? (510 - r3) : r3;

        /*int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        Cube.position.x = tanf(sr::degToRad((((float)mouse_x - window_size.x/2)/window_size.x) * FOV)) * Cube.position.z;
        Cube.position.y = tanf(sr::degToRad((((float)mouse_y - window_size.y/2)/window_size.y) * FOV)) * Cube.position.z;*/

        std::vector<sr::mesh*> mesh_collection;
        mesh_collection.push_back(&Cube);
        sr::Render(setpixel, mesh_collection, Projection_Matrix, Camera, buffers, light, sr::ivec2((int)window_size.x, (int)window_size.y), true);
    

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
