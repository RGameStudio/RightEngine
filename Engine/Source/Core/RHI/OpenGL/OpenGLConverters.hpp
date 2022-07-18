#pragma once

#include "Sampler.hpp"
#include "Texture.hpp"
#include <glad/glad.h>

namespace RightEngine
{
    class OpenGLConverters
    {
    public:
        static GLenum GLSamplerFilter(SamplerFilter minFilter, SamplerFilter mipmapFilter, bool isMipMapped);
        static GLenum GLSamplerFilter(SamplerFilter magFilter);

        static GLenum GLTextureType(TextureType type);
        static GLenum GLCubemapFace(uint8_t face);
    };
}