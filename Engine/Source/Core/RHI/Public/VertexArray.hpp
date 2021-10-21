#pragma once

#include <cstdint>
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

namespace RightEngine
{
    class VertexArray
    {
    public:
        VertexArray();

        ~VertexArray();

        void Bind() const;

        void UnBind() const;

        void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;
    private:
        uint32_t id;
    };
}
