#pragma once

#include <Window.hpp>
#include "Core.h"

extern bool GShouldStop;

class GameApplication
{
public:
    static void OnStart();
    static void OnUpdate();
    static void OnDestroy();
};

int main()
{
    RightEngine::LaunchEngine::Init();
    GameApplication::OnStart();
    RightEngine::Window* window = RightEngine::Window::Create("RightEngine2D", 800, 600);
    while (!GShouldStop)
    {
        window->OnUpdate();
        GameApplication::OnUpdate();
    }
    GameApplication::OnDestroy();
    RightEngine::LaunchEngine::Exit();
    return 0;
}