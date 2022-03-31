#pragma once

#include "Event.hpp"
#include "Types.hpp"

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
        static bool IsMouseButtonUp(MouseButton button);
        static bool IsMouseButtonDown(MouseButton button);

        Input(const Input& input) = delete;
        Input& operator=(const Input& input) = delete;
        Input(const Input&& input) = delete;
        Input& operator=(const Input&& input) = delete;
    private:
        static float lastFrame;
        static bool keyArray[512];
        static bool mouseArray[8];

        static void Init();
        static void OnUpdate();
        bool OnEvent(const Event& inputEvent);
        static Input& Get();

        Input();
        ~Input() = default;

        friend class Application;
        friend class LaunchEngine;
    };
}