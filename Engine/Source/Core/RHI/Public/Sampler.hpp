#pragma once

#include <cstdint>
#include <memory>

namespace RightEngine
{
    enum class SamplerFilter
    {
        Nearest,
        Linear
    };

    struct SamplerDescriptor
    {
        SamplerFilter minFilter;
        SamplerFilter magFilter;
        SamplerFilter mipMapFilter;
        bool isMipMapped;
    };

    class Sampler
    {
    public:
        virtual ~Sampler() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;

        const SamplerFilter minFilter;
        const SamplerFilter magFilter;
        const SamplerFilter mipMapFilter;

        const bool isMipMapped;

        static std::shared_ptr<Sampler> Create(const SamplerDescriptor& desc);

    protected:
        Sampler(const SamplerDescriptor& desc);
    };
}
