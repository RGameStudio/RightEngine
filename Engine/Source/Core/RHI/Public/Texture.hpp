#pragma once

#include "Assert.hpp"
#include <cstdint>
#include <string>
#include <memory>

namespace RightEngine
{
    enum class TextureFormat
    {
        None = 0,
        RED8,
        RGB8,
        RGBA8,
        RGB16F,
        RGB32F
    };

    struct TextureSpecification
    {
        int width{ 0 };
        int height{ 0 };
        int componentAmount{ 0 };
        TextureFormat format{ TextureFormat::None };

        /**
         * @return Texture size in bytes
         */
        inline size_t GetTextureSize() const
        {
            R_CORE_ASSERT(format != TextureFormat::None
                          && width > 0
                          && height > 0
                          && componentAmount > 0, "");
            size_t size = width * height * componentAmount;
            switch (format)
            {
                case TextureFormat::RED8:
                case TextureFormat::RGB8:
                case TextureFormat::RGBA8:
                    return sizeof(uint8_t) * size;
                case TextureFormat::RGB16F:
                    return sizeof(float) / 2 * size;
                case TextureFormat::RGB32F:
                    return sizeof(float) * size;
                default:
                    return 0;
            }
        }
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void UnBind() const = 0;

        inline int GetWidth() const
        { return specification.width; }

        inline int GetHeight() const
        { return specification.height; }

        inline uint32_t GetId() const
        { return id; }

        static std::shared_ptr<Texture> Create(const std::string& path);
        static std::shared_ptr<Texture> Create(const TextureSpecification& aSpecification, const void* data);

    protected:
        TextureSpecification specification;
        uint32_t id;
    };
}