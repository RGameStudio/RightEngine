#include <ShutdownEvent.hpp>
#include "Logger.hpp"
#include "LaunchEngine.hpp"
#include "EventDispatcher.hpp"

bool GShouldStop = true;

void RightEngine::LaunchEngine::Init()
{
    Log::Init();
    GShouldStop = false;
}

void RightEngine::LaunchEngine::Exit()
{
    EventDispatcher::Destroy();
    R_CORE_INFO("Exiting engine!");
}
