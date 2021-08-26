#pragma once

#include <string>
#include <utility>

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

    static Window* Create(std::string title, uint32_t width, uint32_t height);

protected:

    Window(std::string title, uint32_t width, uint32_t height) : title(std::move(title)), width(width), height(height)
    {}

    std::string title;
    uint32_t width;
    uint32_t height;
};