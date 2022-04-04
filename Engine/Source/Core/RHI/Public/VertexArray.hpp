#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include <cstdint>
#include <memory>

namespace RightEngine
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void UnBind() const;

        const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const;
        const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const;

        void AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout);
        void AddBuffer(const std::shared_ptr<IndexBuffer>& ib);
    private:
        uint32_t id;

        std::shared_ptr<VertexBuffer> vertexBuffer;
        std::shared_ptr<IndexBuffer> indexBuffer;
    };
}
