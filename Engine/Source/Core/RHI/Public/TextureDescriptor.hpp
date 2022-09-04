#pragma once

namespace RightEngine
{
    struct TextureDescriptor
    {
        int width{ 0 };
        int height{ 0 };
        int componentAmount{ 0 };
        TextureType type{ TextureType::NONE };
        Format format;

        /**
         * @return Texture size in bytes
         */
        inline size_t GetTextureSize() const
        {
            R_CORE_ASSERT(format != Format::NONE
                          && width > 0
                          && height > 0
                          && componentAmount > 0, "");
            size_t size = width * height * componentAmount;
            switch (format)
            {
                case Format::R8_UINT:
                case Format::RGB8_UINT:
                case Format::RGBA8_UINT:
                    return sizeof(uint8_t) * size;
                case Format::RGB16_SFLOAT:
                    return sizeof(float) / 2 * size;
                case Format::RGBA32_SFLOAT:
                    return sizeof(float) * size;
                case Format::RGBA8_SRGB:
                    return sizeof(uint8_t) * size;
                default:
                R_CORE_ASSERT(false, "");
                    return 0;
            }
        }

        inline bool operator==(const TextureDescriptor& otherSpec)
        {
            return width == otherSpec.width
                   && height == otherSpec.height
                   && componentAmount == otherSpec.componentAmount
                   && format == otherSpec.format;
        }
    };

    struct CubemapFaces
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
}
