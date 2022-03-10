#pragma once

#include "Event.hpp"

namespace RightEngine
{
    class Input
    {
    public:
        static float deltaTime;
        static float frameTime;
        static float GetTime();
        static bool IsKeyDown(int keyCode);
        static bool IsKeyUp(int keyCode);

        Input(const Input& input) = delete;
        Input& operator=(const Input& input) = delete;
        Input(const Input&& input) = delete;
        Input& operator=(const Input&& input) = delete;
    private:
        static float lastFrame;
        static bool keyArray[512];

        static void Init();
        static void OnUpdate();
        bool OnEvent(const Event& event);
        static Input& Get();

        Input();
        ~Input() = default;

        friend class Application;
        friend class LaunchEngine;
    };
}