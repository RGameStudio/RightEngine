#pragma once

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
    while (!GShouldStop)
    {
        GameApplication::OnUpdate();
    }
    GameApplication::OnDestroy();
    RightEngine::LaunchEngine::Exit();
    return 0;
}