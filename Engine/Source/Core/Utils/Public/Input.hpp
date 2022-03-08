#pragma once

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
        static float lastFrame;
    };
}