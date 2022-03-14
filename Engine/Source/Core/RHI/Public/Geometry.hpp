#pragma once

#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Material.hpp"
#include <memory>

namespace RightEngine
{
    class Geometry
    {
    public:
        Geometry();

        void CreateVertexBuffer(const void* data, uint32_t size, int drawMode = GL_STATIC_DRAW);
        void CreateIndexBuffer(const uint32_t * data, uint32_t count, int drawMode = GL_STATIC_DRAW);
        void CreateVertexArray(const RightEngine::VertexBufferLayout& layout);

        const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const;
        const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const;
        const std::shared_ptr<VertexArray>& GetVertexArray() const;

        const std::shared_ptr<Material>& GetMaterial() const;
        void SetMaterial(const std::shared_ptr<Material>& newMaterial);
        
    private:
        std::shared_ptr<VertexBuffer> vertexBuffer;
        std::shared_ptr<IndexBuffer> indexBuffer;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Material> material;
    };
}