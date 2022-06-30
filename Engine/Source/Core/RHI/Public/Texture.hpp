#pragma once

#include "Assert.hpp"
#include "Sampler.hpp"
#include <cstdint>
#include <string>
#include <memory>

namespace RightEngine
{
    enum class TextureType
    {
        NONE,
        TEXTURE_2D,
        CUBEMAP
    };
    enum class TextureFormat
    {
        NONE = 0,

        // 1 channel
        RED8,

        // 2 channels
        RG32F,

        // 3 channels
        RGB8,
        RGB16F,
        RGB32F,

        // 4 channels
        RGBA8,
    };

    struct TextureSpecification
    {
        int width{ 0 };
        int height{ 0 };
        int componentAmount{ 0 };
        TextureType type{ TextureType::NONE };
        TextureFormat format{ TextureFormat::NONE };

        /**
         * @return Texture size in bytes
         */
        inline size_t GetTextureSize() const
        {
            R_CORE_ASSERT(format != TextureFormat::NONE
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

        inline bool operator==(const TextureSpecification& otherSpec)
        {
            return width == otherSpec.width
                   && height == otherSpec.height
                   && componentAmount == otherSpec.componentAmount
                   && format == otherSpec.format;
        }
    };

    struct CubeMapFaces
    {
        std::vector<uint8_t> face1;
        std::vector<uint8_t> face2;
        std::vector<uint8_t> face3;
        std::vector<uint8_t> face4;
        std::vector<uint8_t> face5;
        std::vector<uint8_t> face6;

        const std::vector<uint8_t>& GetFaceData(uint32_t index) const
        {
            R_CORE_ASSERT(index >= 0 && index < 6, "");
            switch (index)
            {
                case 0:
                    return face1;
                case 1:
                    return face2;
                case 2:
                    return face3;
                case 3:
                    return face4;
                case 4:
                    return face5;
                case 5:
                    return face6;
            }
        }

        void SetFaceData(const std::vector<uint8_t>& data, uint32_t index)
        {
            R_CORE_ASSERT(index >= 0 && index < 6, "");
            switch (index)
            {
                case 0:
                R_CORE_ASSERT(face1.empty(), "");
                    face1 = data;
                    break;
                case 1:
                R_CORE_ASSERT(face2.empty(), "");
                    face2 = data;
                    break;
                case 2:
                R_CORE_ASSERT(face3.empty(), "");
                    face3 = data;
                    break;
                case 3:
                R_CORE_ASSERT(face4.empty(), "");
                    face4 = data;
                    break;
                case 4:
                R_CORE_ASSERT(face5.empty(), "");
                    face5 = data;
                    break;
                case 5:
                R_CORE_ASSERT(face6.empty(), "");
                    face6 = data;
                    break;
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

        inline std::shared_ptr<Sampler> GetSampler() const
        { return sampler; }

        void SetSampler(const std::shared_ptr<Sampler>& aSampler)
        { sampler = aSampler; }

        virtual void GenerateMipmaps() const = 0;

        static std::shared_ptr<Texture> Create(const TextureSpecification& aSpecification,
                                               const std::vector<uint8_t>& data);
        static std::shared_ptr<Texture> Create(const TextureSpecification& aSpecification,
                                               const std::array<std::vector<uint8_t>, 6>& data);

    protected:
        uint32_t id;
        TextureSpecification specification;
        std::shared_ptr<Sampler> sampler;
    };
}