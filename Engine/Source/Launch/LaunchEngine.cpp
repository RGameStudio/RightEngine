#include "Logger.hpp"
#include "LaunchEngine.hpp"
#include "EventDispatcher.hpp"

void RightEngine::LaunchEngine::Init()
{
    Log::Init();
}

void RightEngine::LaunchEngine::Exit()
{
    EventDispatcher::Destroy();
    R_CORE_INFO("Exiting engine!");
}
