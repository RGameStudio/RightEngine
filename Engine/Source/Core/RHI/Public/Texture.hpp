#pragma once

#include <cstdint>
#include <string>

namespace RightEngine
{
    class Texture
    {
    public:
        explicit Texture(const std::string& path);
        ~Texture();

        void Bind(uint32_t slot = 0) const;
        void UnBind() const;

        inline int GetWidth() const
        { return width; }

        inline int GetHeight() const
        { return height; }

    private:
        int width, height, componentAmount;
        uint32_t id;
    };
}