#pragma once

#include "Sampler.hpp"
#include <glad/glad.h>

namespace RightEngine
{
    class OpenGLConverters
    {
    public:
        static GLenum samplerFilter(SamplerFilter minFilter, SamplerFilter mipmapFilter, bool isMipMapped);
        static GLenum samplerFilter(SamplerFilter magFilter);
    };
}