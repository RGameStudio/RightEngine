#include <ShutdownEvent.hpp>
#include "Logger.hpp"
#include "LaunchEngine.hpp"
#include "EventDispatcher.hpp"

bool GShouldStop = true;

namespace RightEngine
{
    void LaunchEngine::Init()
    {
        Log::Init();
        launchContext = new LaunchContext();
        GShouldStop = false;
    }

    void LaunchEngine::Exit()
    {
        EventDispatcher::Destroy();
        R_CORE_INFO("Exiting engine!");

        delete launchContext;
    }

    LaunchEngine::LaunchContext::LaunchContext()
    {
        EventDispatcher::Get()->Subscribe(ShutdownEvent::descriptor, EVENT_CALLBACK(LaunchEngine::LaunchContext::OnEvent));
    }

    bool LaunchEngine::LaunchContext::OnEvent(const Event& event)
    {
        if (event.GetType() == ShutdownEvent::descriptor)
        {
            GShouldStop = true;
        }

        return true;
    }

    LaunchEngine::LaunchContext* LaunchEngine::launchContext = nullptr;
}