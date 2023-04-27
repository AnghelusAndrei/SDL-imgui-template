#include "app.hpp"
#include "SDL_net.h"

App::App(){
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);


    server = new Server(2000);
}

App::~App(){
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    delete server;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

ImVec2 App::GetWindowSize(){
    std::pair<int, int> size;
    SDL_GetWindowSize(window, &size.first, &size.second);
    return ImVec2(size.first, size.second);
}

bool App::Frame(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            return 0;
        if (event.type == SDL_QUIT)
            return 0;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            return 0;
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    server->Listen();
    Log log = server->GetLog();

    server_package s_package;

    {
        ImVec2 window_size = GetWindowSize();
        //ImGui::SetNextWindowPos(ImVec2(0,0));
        //ImGui::SetNextWindowSize(window_size);

        ImGui::Begin("Server");  

        ImGui::Spacing();


        ImGui::SeparatorText("Server data");

        uint8_t parts[4] = {0};
        for (int i=0; i<4 ;++i)
            parts[i] = ((uint8_t*)&server->ip.host)[3-i];
        ImGui::Text("The server is ran on port %i and local ip %i.%i.%i.%i", server->port, (int)parts[3], (int)parts[2], (int)parts[1], (int)parts[0]); 
        ImGui::Text("connected clients: %i", server->clients.size());

        ImGui::SeparatorText("Clients");

        ImGui::BeginChild("clients", ImVec2(0, 60), true);

        bool updates = false;
        for (auto client : server->clients){
            std::pair<client_package*, bool> c_package = server->GetPackage(client.first);

            if(c_package.second){
                std::strcpy(s_package.text, c_package.first->text);
                updates = true;
            }

            for (int i=0; i<4 ;++i)
                parts[i] = ((uint8_t*)&client.second.host)[3-i];

            ImGui::BulletText("Client id: host: %i.%i.%i.%i, port: %i. properites: name:", (int)parts[3], (int)parts[2], (int)parts[1], (int)parts[0], client.second.port);
            ImGui::SameLine();
            ImGui::Text(c_package.first->name);
        }
        s_package.num_users = server->clients.size();
        if(updates){
            for (auto client : server->clients){
                server->SendPackage(client.first, &s_package);
            }
        }

        ImGui::EndChild();

        ImGui::End();
    }

    log.Draw("log");
    

    // Rendering
    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);

    return 1;
}