#pragma once
#include "../imgui/internal/imgui.h"
#include "../imgui/internal/imgui_impl_sdl2.h"
#include "../imgui/internal/imgui_impl_sdlrenderer.h"

class Log
{
public:

    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    Log();

    void    Clear();
    void    AddLog(const char* fmt, ...) ;
    void    Draw(const char* title, bool* p_open = NULL);
};