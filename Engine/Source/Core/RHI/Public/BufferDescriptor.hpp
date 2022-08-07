#pragma once

#include "Types.hpp"

namespace RightEngine
{
    enum BufferType
    {
        BUFFER_TYPE_VERTEX = BIT(0),
        BUFFER_TYPE_TRANSFER_SRC = BIT(1),
        BUFFER_TYPE_TRANSFER_DST = BIT(2),
        BUFFER_TYPE_INDEX = BIT(3),
    };

    struct BufferDescriptor
    {
        size_t size;
        BufferType type;
        MemoryType memoryType;
    };
}
