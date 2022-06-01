#pragma once

#include "Texture.hpp"
#include <array>

namespace RightEngine
{
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

    class Texture3D : public Texture
    {
    public:
        static std::shared_ptr<Texture3D> Create(const std::array<std::string, 6>& texturesPath);
    };
}
