#pragma once

namespace RightEngine
{
    enum class SamplerFilter
    {
        Nearest = 0,
        Linear
    };

    enum class AddressMode
    {
	    Repeat = 0,
        ClampToEdge,
        ClampToBorder
    };

    struct SamplerDescriptor
    {
        SamplerFilter minFilter;
        SamplerFilter magFilter;
        SamplerFilter mipMapFilter;
        AddressMode addressModeU = AddressMode::Repeat;
        AddressMode addressModeV = AddressMode::Repeat;
        AddressMode addressModeW = AddressMode::Repeat;
        bool isMipMapped;
    };
}
