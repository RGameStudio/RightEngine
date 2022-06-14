#pragma once

#include "Texture3D.hpp"

namespace RightEngine
{
    class OpenGLTexture3D : public Texture3D
    {
    public:
        OpenGLTexture3D(const std::array<std::string, 6>& texturesPath);
        OpenGLTexture3D(const TextureSpecification& textureSpecification,
                        const std::array<std::vector<uint8_t>, 6>& data);
        virtual ~OpenGLTexture3D() override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;

    private:
        void Generate(const CubeMapFaces& faces);
    };
}