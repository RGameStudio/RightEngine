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

    enum class Format
    {
        R32_SFLOAT,
        R32_UINT,
        R8_UINT,
        R32G32_SFLOAT,
        R32G32B32_SFLOAT,
        R32G32B32A32_SFLOAT,
        B8G8R8A8_SRGB,
    };

    enum class PresentMode
    {
        IMMEDIATE,
        FIFO,
        MAILBOX
    };

    #define BIT(x) 1 << x

    enum MemoryType
    {
        MEMORY_TYPE_HOST_VISIBLE = BIT(0),
        MEMORY_TYPE_HOST_COHERENT = BIT(1),
        MEMORY_TYPE_DEVICE_LOCAL = BIT(2)
    };
}
