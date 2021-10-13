#pragma once

#include "IndexBuffer.hpp"
#include "VertexArray.hpp"

namespace RightEngine
{
    class Renderer
    {
    public:
        static void Draw(const VertexArray& va, const IndexBuffer& ib);
        static void Draw(const VertexArray& va, const VertexBuffer& vb);
    };
}