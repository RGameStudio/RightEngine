#pragma once

#include <vector>
#include <cassert>
#include <glad/glad.h>

namespace RightEngine
{
    struct VertexBufferElement
    {
        uint32_t type;
        uint32_t count;
        uint8_t normalized;

        static unsigned int GetSizeOfType(unsigned int type);
    };

    class VertexBufferLayout
    {
    private:
        std::vector<VertexBufferElement> elements;
        uint32_t stride;
    public:
        VertexBufferLayout() : stride(0)
        {

        }

        template<typename T>
        void Push(uint32_t count, bool normalized = false)
        {
            static_assert(true, "");
        }

        inline const std::vector<VertexBufferElement> GetElements() const &
        { return elements; }

        inline unsigned int GetStride() const
        { return stride; }
    };

    template<>
    inline void VertexBufferLayout::Push<float>(uint32_t count, bool normalized)
    {
        elements.push_back({GL_FLOAT, count, (uint8_t) (normalized ? GL_TRUE : GL_FALSE)});
        stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
    }

    template<>
    inline void VertexBufferLayout::Push<uint32_t>(uint32_t count, bool normalized)
    {
        elements.push_back({GL_UNSIGNED_INT, count, (uint8_t) (normalized ? GL_TRUE : GL_FALSE)});
        stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
    }

    template<>
    inline void VertexBufferLayout::Push<uint8_t>(uint32_t count, bool normalized)
    {
        elements.push_back({GL_UNSIGNED_BYTE, count, (unsigned char) (normalized ? GL_TRUE : GL_FALSE)});
        stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
    }
}