#include "VertexBufferLayout.hpp"

unsigned int RightEngine::VertexBufferElement::GetSizeOfType(Format format)
{
    switch (format) {
        case Format::B8G8R8A8_SRGB:
            return 1;
        case Format::R32G32_SFLOAT:
            return 8;
        case Format::R32_SFLOAT:
            return 4;
        case Format::R32G32B32_SFLOAT:
            return 12;
        case Format::R32G32B32A32_SFLOAT:
            return 16;
    }
    assert(false);
    return 0;
}

