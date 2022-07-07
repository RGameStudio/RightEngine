#pragma once

#include "Texture.hpp"
#include <glad/glad.h>

namespace RightEngine
{

    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const TextureSpecification& aSpecification, const std::vector<uint8_t>& data);
        OpenGLTexture(const TextureSpecification& aSpecification, const CubemapFaces& faces);
        virtual ~OpenGLTexture() override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;

        virtual void GenerateMipmaps() const override;

    private:
        void Create();
        void Generate(const void* buffer);
        void Generate(const CubemapFaces& faces);
        void GenerateTexture(const void* buffer, GLenum type);
        inline void ValidateTextureData() const;
    };
}
