#pragma once

#include <Window.hpp>
#include <GLFW/glfw3.h>

namespace RightEngine
{
    class GLFWWindow : public Window
    {
    public:
        virtual void OnUpdate() override;

        void Swap() const override;

        virtual void* GetNativeHandle() const override;

        GLFWWindow(const std::string& title, uint32_t width, uint32_t height);

        ~GLFWWindow();
    private:
        void Init(const std::string& title, uint32_t width, uint32_t height);
        GLFWwindow *window = nullptr;
    };
}