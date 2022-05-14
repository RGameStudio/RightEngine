#pragma once

#include <cstdint>
#include <string>

namespace RightEngine
{
    enum class TextureFormat
    {
        None = 0,
        RGBA8,
        RGB16F
    };

    struct TextureSpecification
    {
        int width{ 0 };
        int height{ 0 };
        int componentAmount{ 0 };
        TextureFormat format{ TextureFormat::RGBA8 };
    };

    class Texture
    {
    public:
        explicit Texture(const std::string& path);
        explicit Texture(const TextureSpecification& specification, const void* data);
        ~Texture();

        void Bind(uint32_t slot = 0) const;
        void UnBind() const;

        inline int GetWidth() const
        { return specification.width; }

        inline int GetHeight() const
        { return specification.height; }

        inline uint32_t GetId() const
        { return id; }

    private:
        TextureSpecification specification;
        uint32_t id;

    private:
        void Generate(const void* data);
    };
}