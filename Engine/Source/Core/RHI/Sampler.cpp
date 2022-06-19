#include "Sampler.hpp"
#include "Renderer.hpp"
#include "OpenGL/OpenGLSampler.hpp"

using namespace RightEngine;

std::shared_ptr<Sampler> Sampler::Create(const SamplerDescriptor& desc)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLSampler>(desc);
        default:
        R_CORE_ASSERT(false, "Unknown GPU API!");
    }
}

Sampler::Sampler(const SamplerDescriptor& desc) :
        isMipMapped(desc.isMipMapped),
        minFilter(desc.minFilter),
        magFilter(desc.magFilter),
        mipMapFilter(desc.mipMapFilter)
{}
