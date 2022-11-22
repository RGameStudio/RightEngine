#pragma once

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
}
