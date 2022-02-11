#pragma once

#include <string>
#include <utility>

namespace RightEngine
{
    class Window
    {
    public:

        inline uint32_t GetWidth() const
        {
            return width;
        }

        inline uint32_t GetHeight() const
        {
            return height;
        }

        virtual void OnUpdate() = 0;

        virtual void Swap() const = 0;

        virtual void* GetNativeHandle() const = 0;

        static Window *Create(std::string title, uint32_t width, uint32_t height);

    protected:

        Window(const std::string& title, uint32_t width, uint32_t height) : title(title), width(width),
                                                                     height(height)
        {}

        std::string title;
        uint32_t width;
        uint32_t height;
    };
}