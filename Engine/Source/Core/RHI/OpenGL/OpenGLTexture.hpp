#pragma once

#include "Texture.hpp"

namespace RightEngine
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(const TextureSpecification& aSpecification, const void* data);
        virtual ~OpenGLTexture() override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;
    private:
        void Generate(const void* buffer);
    };
}
