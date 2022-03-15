#include "VertexBufferLayout.hpp"

unsigned int RightEngine::VertexBufferElement::GetSizeOfType(unsigned int type)
{
    switch (type) {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        case GL_UNSIGNED_INT:
            return 4;
    }
    assert(false);
    return 0;
}

