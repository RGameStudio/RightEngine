#include "OpenGLSampler.hpp"
#include "OpenGLConverters.hpp"
#include <glad/glad.h>

using namespace RightEngine;

OpenGLSampler::OpenGLSampler(const SamplerDescriptor& desc) : Sampler(desc)
{
    glCreateSamplers(1, &id);
    Init();
}

void OpenGLSampler::Init()
{
    // TODO: Add texture address mode and texture max and min LOD
    glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, OpenGLConverters::samplerFilter(minFilter, mipMapFilter, isMipMapped));
    glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, OpenGLConverters::samplerFilter(magFilter));

    glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glSamplerParameterf(id, GL_TEXTURE_MIN_LOD, 0.0f);
    glSamplerParameterf(id, GL_TEXTURE_MAX_LOD, 4.0f);

//    glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

OpenGLSampler::~OpenGLSampler()
{
    glDeleteSamplers(1, &id);
}

void OpenGLSampler::Bind(uint32_t slot) const
{
    glBindSampler(slot, id);
}
