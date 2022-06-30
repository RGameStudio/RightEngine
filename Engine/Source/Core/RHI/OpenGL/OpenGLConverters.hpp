#pragma once

#include "Sampler.hpp"
#include "Texture.hpp"
#include <glad/glad.h>

namespace RightEngine
{
    class OpenGLConverters
    {
    public:
        static GLenum samplerFilter(SamplerFilter minFilter, SamplerFilter mipmapFilter, bool isMipMapped);
        static GLenum samplerFilter(SamplerFilter magFilter);

        static GLenum textureType(TextureType type);
        static GLenum cubeMapFace(uint8_t face);
    };
}