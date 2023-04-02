#include "app.hpp"
#include "engine.h"

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
    window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    const float FOV = 90;
    const float Zfar = 1000;
    const float Znear = 0.1;
    mat4x4 Projection_Matrix = Matrix_Projection(1280, 720, FOV, Zfar, Znear);
    float *pDepthBuffer = new float[1280 * 720];

    mesh Cube;
    Cube.LoadFile("./assets/sphere.obj", false);

    player_t Camera = {0,0,0,0,0};

    vec3 light;
    light.x = -1;
    light.y = -0.75;
    light.z = -0.5;

    Cube.position.x = 0;
    Cube.position.y = 0;
    Cube.position.z = 5;

    Cube.size.x = 1;
    Cube.size.y = 1;
    Cube.size.z = 1;

    Cube.color = {50, 100, 255};

    stage = LOGIN;


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        switch (stage)
        {
        case 1:Login();break;
        case 2:Home();break;
        case 3:Request();break;
        case 4:Inbox();break;
        }


        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);

        Cube.rotation.x = (float)(SDL_GetTicks()/50);
        Cube.rotation.y = (float)(SDL_GetTicks()/50);
        Cube.rotation.z = (float)(SDL_GetTicks()/50);

        std::vector<mesh> mesh_collection;
        mesh_collection.push_back(Cube);
        Frame(renderer, mesh_collection, Projection_Matrix, Camera, pDepthBuffer, light, 1280, 720);

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }
}

App::~App(){
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::SetWidgetStyle(){
    const ImVec2 pos = ImVec2(1280 / 3, 720 / 5);
    const ImVec2 size = ImVec2(1280 / 3, 720 / 1.5);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowCollapsed(false);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(0.6f);
}

void App::TextCentered(std::string text) {
    float win_width = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;

    // calculate the indentation that centers the text on one line, relative
    // to window left, regardless of the ImGuiStyleVar_WindowPadding value
    float text_indentation = (win_width - text_width) * 0.5f;

    // if text is too long to be drawn on one line, text_indentation can
    // become too small or even negative, so we check a minimum indentation
    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation) {
        text_indentation = min_indentation;
    }

    ImGui::SameLine(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopTextWrapPos();
}

void App::Login(){
    SetWidgetStyle();

    ImGui::Begin("Login"); 

    static char username[128] = "";
    static char password[128] = "";

    TextCentered("Input password and username here");
    ImGui::Text("Username: "); ImGui::SameLine(); ImGui::InputText("username", username, IM_ARRAYSIZE(username));
    ImGui::Text("Password: "); ImGui::SameLine(); ImGui::InputText("password", password, IM_ARRAYSIZE(password));

    if(ImGui::Button("Submit"))
        stage++;

    ImGui::End();
}

void App::Home(){
    SetWidgetStyle();

    ImGui::Begin("Home"); 

    TextCentered("Home");

    if(ImGui::Button("Request"))
        stage = REQUEST;
    
    if(ImGui::Button("Inbox"))
        stage = INBOX;

    ImGui::End();
}

void App::Request(){
    SetWidgetStyle();

    ImGui::Begin("Request"); 

    TextCentered("Request");

    if(ImGui::Button("Submit"))
        stage = HOME;

    if(ImGui::Button("Home"))
        stage = HOME;

    if(ImGui::Button("Inbox"))
        stage = INBOX;

    ImGui::End();
}
void App::Inbox(){
    SetWidgetStyle();

    ImGui::Begin("Inbox"); 

    TextCentered("View requests here:");

    if(ImGui::Button("Home"))
        stage = HOME;

    if(ImGui::Button("Request"))
        stage = REQUEST;

    ImGui::End();
}