#pragma once

#ifdef R_TEST_BUILD
#include <gtest/gtest.h>
#endif

#include <Window.hpp>
#include <Application.hpp>
#include "Core.h"

enum class LaunchMode;

extern bool GShouldStop;
extern LaunchMode GLaunchMode;

class GameApplication
{
public:
    static void OnStart();

    static void OnUpdate();

    static void OnDestroy();
};

int main(int argc, char *argv[])
{
#ifdef R_TEST_BUILD
    GLaunchMode = LaunchMode::Test;
#endif
    RightEngine::LaunchEngine::Init(argc, argv);
    RightEngine::Application *application = RightEngine::Application::Get();

    if (GLaunchMode == LaunchMode::Game)
    {
        GameApplication::OnStart();

        while (!GShouldStop)
        {
            application->OnUpdate();
            GameApplication::OnUpdate();
            application->OnUpdateEnd();
        }

        GameApplication::OnDestroy();
    }
#ifdef R_TEST_BUILD
    else if (GLaunchMode == LaunchMode::Test)
    {
        ::testing::InitGoogleTest(&argc, argv);
        RUN_ALL_TESTS();
    }
#endif

    RightEngine::LaunchEngine::Exit();
    return 0;
}