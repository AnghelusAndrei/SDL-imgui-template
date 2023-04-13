#include "interface.hpp"
#include "engine/sr_impl.hpp"
#include "logger/logger.hpp"

Interface::Interface(Client *client_) : client(client_){
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

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    server = new server_package();


}

Interface::~Interface(){
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

ImVec2 Interface::GetWindowSize(){
    std::pair<int, int> size;
    SDL_GetWindowSize(window, &size.first, &size.second);
    return ImVec2(size.first, size.second);
}

bool Interface::frame(){
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

    sr::mesh Cube;
    sr::player_t Camera = {0,0,0,0,0};
    sr::vec3 light;

    Cube.LoadFile("./assets/sphere.obj", false);

    light.x = -1;
    light.y = -0.75;
    light.z = -0.5;

    Cube.position.x = 0;
    Cube.position.y = 0;
    Cube.position.z = 5;

    Cube.size.x = 1;
    Cube.size.y = 1;
    Cube.size.z = 1;

    Cube.color = {255, 100, 50};

    ImVec2 window_size = GetWindowSize();

    float FOV = 60;
    float Zfar = 1000;
    float Znear = 0.1;
    sr::mat4x4 Projection_Matrix = sr::Matrix_Projection((int)window_size.x, (int)window_size.y, FOV, Zfar, Znear);
    float *pDepthBuffer = new float[(int)window_size.x * (int)window_size.y];



    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //get data from client thread
    debug_data debug = client->GetDebug();
    IPaddress address = client->GetIP();
    server = client->s_package();

    {
        
        //ImGui::SetNextWindowPos(ImVec2(0,0));
        //ImGui::SetNextWindowSize(window_size);

        ImGui::Begin("Client");  

        ImGui::Spacing();

        if(!debug.debug_error){
            ImGui::Text("debug:"); ImGui::SameLine(); 
            if(debug.debug_init){
                ImGui::Text(debug.debug);
            }else{
                ImGui::Text("None");
            }
        }else{
            ImGui::Text("error:"); ImGui::SameLine(); ImGui::Text(debug.debug);
        }


        ImGui::SeparatorText("Server data");

        uint8_t parts[4] = {0};
        for (int i=0; i<4 ;++i)
            parts[i] = ((uint8_t*)&address.host)[3-i];
        ImGui::Text("Established server connection on server port %i and local ip %i.%i.%i.%i", address.port, (int)parts[3], (int)parts[2], (int)parts[1], (int)parts[0]); 
        ImGui::Text("users connected: %i", server->num_users);
        
        ImGui::BeginChild("Interface", ImVec2(0, 40), true);
        ImGui::Text("Input name here:");
        ImGui::SameLine();

        //should be safe
        /*
        reading without using any mutex keeps memory coherency,
        but may have non coherent results
        */
        ImGui::InputText(client->data->c_package->name, input_name, IM_ARRAYSIZE(input_name));

        ImGui::SameLine();
        if(ImGui::Button("Submit", ImVec2(0,0)) && std::strlen(input_name) > 0){
            std::strcpy(data.name, input_name);
            client->Send(&data);
        }

        ImGui::EndChild();

        ImGui::BeginChild("chat", ImVec2(0, 0), true);
        for(auto text : server->text){
            ImGui::Text(text);
        }

        ImGui::InputText("text", input_text, IM_ARRAYSIZE(input_text));

        ImGui::SameLine();
        if(ImGui::Button("Submit", ImVec2(0,0)) && std::strlen(input_text) > 0){
            std::strcpy(data.text, input_text);
            client->Send(&data);
        }

        ImGui::EndChild();

        ImGui::End();
    }

    bool open;
    ImGui::ShowDemoWindow(&open);
    

    // Rendering
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

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    Cube.position.x = tanf(sr::degToRad((((float)mouse_x - window_size.x/2)/window_size.x) * FOV)) * Cube.position.z;
    Cube.position.y = tanf(sr::degToRad((((float)mouse_y - window_size.y/2)/window_size.y) * FOV)) * Cube.position.z;

    std::vector<sr::mesh> mesh_collection;
    mesh_collection.push_back(Cube);
    sr::Frame(renderer, mesh_collection, Projection_Matrix, Camera, pDepthBuffer, light, (int)window_size.x, (int)window_size.y, true);
    

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);

    delete pDepthBuffer;

    return 1;
}