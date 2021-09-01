#pragma once

#include <memory>
#include "Window.hpp"

namespace RightEngine
{
    class Application
    {
    public:
        static Application *Get();

        void OnUpdate();

    private:
        static Application *instance;

        Application();

        void Init();

        std::unique_ptr<Window> window;
    };

}