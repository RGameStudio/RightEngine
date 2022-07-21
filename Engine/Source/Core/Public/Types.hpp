#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace RightEngine
{
    enum class GPU_API
    {
        None = 0,
        OpenGL,
        Vulkan
    };

    enum class MouseButton
    {
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };
}
