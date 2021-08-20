#include <Logger.hpp>
#include "LaunchEngine.hpp"

void RightEngine::LaunchEngine::Init()
{
    Log::Init();
}

void RightEngine::LaunchEngine::Exit()
{
    R_CORE_INFO("Exiting engine!");
}
