#include <ShutdownEvent.hpp>
#include "Logger.hpp"
#include "LaunchEngine.hpp"
#include "EventDispatcher.hpp"

bool GShouldStop = true;
LaunchMode GLaunchMode = LaunchMode::Game;

namespace RightEngine
{
    void LaunchEngine::Init(int argc, char* argv[])
    {
        Log::Init();
        launchContext = new LaunchContext();
        launchContext->SetCmdArgs(argc, argv);
        GShouldStop = false;
    }

    void LaunchEngine::Exit()
    {
        R_CORE_INFO("Exiting engine!");

        delete launchContext;
    }

    LaunchEngine::LaunchContext::LaunchContext()
    {
        EventDispatcher::Get().Subscribe(ShutdownEvent::descriptor, EVENT_CALLBACK(LaunchEngine::LaunchContext::OnEvent));
    }

    void LaunchEngine::LaunchContext::SetCmdArgs(int argc, char **argv)
    {
        easyArgs = std::make_unique<EasyArgs>(argc, argv);
        easyArgs->Version("0.0.1");
        easyArgs->Value("-m", "--mode", "Engine launch modes [Game|Test].", false);

        ParseCmdArgs();
    }

    void LaunchEngine::LaunchContext::ParseCmdArgs()
    {
        std::string mode = easyArgs->GetValueFor("-m");
        if (mode == "Game")
        {
            GLaunchMode = LaunchMode::Game;
        }
        else if (mode == "Test")
        {
            GLaunchMode = LaunchMode::Test;
        }
    }

    bool LaunchEngine::LaunchContext::OnEvent(const Event& event)
    {
        if (event.GetType() == ShutdownEvent::descriptor)
        {
            GShouldStop = true;
        }

        return true;
    }

    LaunchEngine::LaunchContext *LaunchEngine::launchContext = nullptr;
}