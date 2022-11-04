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
        NONE,
        R32_SFLOAT,
        R32_UINT,
        R8_UINT,
        RG16_SFLOAT,
        RGB8_UINT,
        RGBA8_UINT,
        RGB16_SFLOAT,
        RG32_SFLOAT,
        RGB32_SFLOAT,
        RGBA16_SFLOAT,
        RGBA32_SFLOAT,
        RGBA16_UNORM,
        RGB16_UNORM,
        BGRA8_SRGB,
        RGBA8_SRGB,
        BGRA8_UNORM,

        //Depth buffer formats
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT
    };

    enum class PresentMode
    {
        IMMEDIATE,
        FIFO,
        MAILBOX
    };

#define BIT(x) 1 << x

    enum class MemoryType
    {
        CPU_ONLY,
        CPU_GPU,
        GPU_ONLY
    };

    enum class TextureType
    {
        NONE,
        TEXTURE_2D,
        CUBEMAP
    };

    enum class RendererStatus
    {
        OK = 0,
        FRAMEBUFFER_OUT_OF_DATE,
    };
}
