#pragma once

#include "Core.h"

extern int Run();

int main()
{
    RightEngine::LaunchEngine::Init();
    int status = Run();
    RightEngine::LaunchEngine::Exit();
    return status;
}