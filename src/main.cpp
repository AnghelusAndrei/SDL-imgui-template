#define SDL_MAIN_HANDLED
#include "imgui/internal/imgui.h"
#include "imgui/internal/imgui_impl_sdl2.h"
#include "imgui/internal/imgui_impl_sdlrenderer.h"
#include <stdio.h>
#include "engine/sr_core.hpp"
#include "SDL.h"
#include "SDL_image.h"


#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

ImVec2 GetWindowSize(SDL_Window *window){
    std::pair<int, int> size;
    SDL_GetWindowSize(window, &size.first, &size.second);
    return ImVec2(size.first, size.second);
}


sr::ivec2 getsize(SDL_Texture *texture) {
    sr::ivec2 size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return size;
}

uint32_t formatRGB(sr::colorRGB color){
    return color.b << 24 | color.r << 8 | color.g << 16;
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

    Cube.LoadFile("./assets/capsule.obj");
    SDL_Surface *obj_surface = IMG_Load("./assets/capsule.jpg");

    Cube.texture = new sr::colorRGB[obj_surface->h * obj_surface->w];
    Cube.texture_size = sr::ivec2(obj_surface->w, obj_surface->h);
    for(int y = 0; y < obj_surface->h; y++){
        for(int x = 0; x < obj_surface->w; x++){
            Cube.texture[y * obj_surface->w + x] = sr::colorRGB(
                ((char*)obj_surface->pixels)[obj_surface->format->BytesPerPixel * (y * obj_surface -> w + x) + 2],
                ((char*)obj_surface->pixels)[obj_surface->format->BytesPerPixel * (y * obj_surface -> w + x) + 1],
                ((char*)obj_surface->pixels)[obj_surface->format->BytesPerPixel * (y * obj_surface -> w + x) + 0]
            );
        }
    }
    
    light.x = -1;
    light.y = -0.75;
    light.z = -0.5;

    sr::vec3 base_light = sr::vec3(-1.0f, -0.75f, -0.5f);

    Cube.position.x = 0;
    Cube.position.y = 0;
    Cube.position.z = 1.3;

    Cube.size.x = 0.4;
    Cube.size.y = 0.4;
    Cube.size.z = 0.4;

    Cube.color = {255, 100, 50};


    SDL_Texture* texture_buffer = SDL_CreateTexture(renderer, 
                            SDL_PIXELFORMAT_BGRA8888,
                            SDL_TEXTUREACCESS_STREAMING, 
                            window_size.x,
                            window_size.y);

    int *pixel_buffer;
    int pitch;
    
    std::function<void(sr::ivec2 pixel, sr::colorRGB color)> setpixel = [&window_size, &pixel_buffer](sr::ivec2 pixel, sr::colorRGB color){
        pixel_buffer[pixel.y * (int)window_size.x + pixel.x] = formatRGB(color);
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
                window_size = GetWindowSize(window);
                buffers->update_framebuffer_size(sr::ivec2((int)window_size.x, (int)window_size.y));
                SDL_DestroyTexture(texture_buffer);
                texture_buffer = SDL_CreateTexture(renderer, 
                            SDL_PIXELFORMAT_BGRA8888,
                            SDL_TEXTUREACCESS_STREAMING, 
                            window_size.x,
                            window_size.y);
            }
        }

        float FOV = 60;
        float Zfar = 1000;
        float Znear = 0.1;
        sr::mat4x4 Projection_Matrix = sr::Matrix_Projection(sr::ivec2((int)window_size.x, (int)window_size.y), FOV, Zfar, Znear);


        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        {
            ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
            ImGui::Begin("Debug profile", NULL, flags);

            ms_f2 = SDL_GetTicks();

            ImGui::Text("frame ms: %i", ms_f2 - ms_f1);
            ImGui::Text("frame fps: %f", 1000.0f / (float)(ms_f2 - ms_f1));

            ms_f1 = ms_f2;

            ImGui::End();
        }

        
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);


        Cube.rotation.x = (float)(SDL_GetTicks()/50);
        Cube.rotation.y = (float)(SDL_GetTicks()/50);
        Cube.rotation.z = (float)(SDL_GetTicks()/50);

        int r1 = (SDL_GetTicks()/20)%410 + 50;
        int r2 = (SDL_GetTicks()/10)%410 + 50;
        int r3 = (SDL_GetTicks()/25)%410 + 50;

        Cube.color.r = r1 > 255 ? (510 - r1) : r1;
        Cube.color.g = r2 > 255 ? (510 - r2) : r2;
        Cube.color.b = r3 > 255 ? (510 - r3) : r3;

        sr::mat4x4 light_rotation_x = sr::Matrix_RotateX((float)(SDL_GetTicks()/50));
        sr::mat4x4 light_rotation_y = sr::Matrix_RotateX((float)(SDL_GetTicks()/50));
        sr::mat4x4 light_rotation_z = sr::Matrix_RotateX((float)(SDL_GetTicks()/50));

        //light = sr::Vector_MultiplyMatrix(base_light, light_rotation);
        light = sr::Vector_Normalise(light);

        std::vector<sr::mesh*> mesh_collection;
        mesh_collection.push_back(&Cube);
        SDL_LockTexture(texture_buffer,
                        NULL,      // NULL means the *whole texture* here.
                        (void**)&pixel_buffer,
                        &pitch);

        uint32_t clear_pixel_col = formatRGB(sr::colorRGB(clear_color.x * 255, clear_color.y * 255, clear_color.z * 255));
        std::fill_n(pixel_buffer, (int)window_size.x*(int)window_size.y, clear_pixel_col);
        sr::Render(setpixel, mesh_collection, Projection_Matrix, Camera, buffers, light, sr::ivec2((int)window_size.x, (int)window_size.y), true);
        SDL_UnlockTexture(texture_buffer);
    
        SDL_RenderCopy(renderer, texture_buffer, NULL, NULL);
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
