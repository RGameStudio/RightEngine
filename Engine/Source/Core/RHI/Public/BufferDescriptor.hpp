#pragma once

namespace RightEngine
{
    enum class BufferType
    {
        VERTEX
    };
    struct BufferDescriptor
    {
        size_t size;
        BufferType type;
    };
}
