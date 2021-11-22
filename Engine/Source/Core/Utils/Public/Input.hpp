#pragma once
#include <cstdint>
#include <chrono>

namespace RightEngine
{
    class Input
    {
    public:
        static float GetTime();
        static void OnUpdate();
        static float deltaTime;
        static float frameTime;
    private:
        static std::chrono::high_resolution_clock::time_point launchTime;
        static float lastFrameSinceStartTime;
        static float lastFrameTime;
    };
}