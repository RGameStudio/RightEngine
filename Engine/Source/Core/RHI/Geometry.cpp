#include "Geometry.hpp"
#include "Renderer.hpp"
#include "Core.h"

using namespace RightEngine;

void RightEngine::Geometry::CreateVertexBuffer(const void* data, uint32_t size, int drawMode)
{
    R_CORE_ASSERT(!vertexBuffer, "Vertex buffer was already created for that geometry");
    vertexBuffer = std::make_shared<VertexBuffer>(data, size, drawMode);
}

void RightEngine::Geometry::CreateIndexBuffer(const uint32_t* data, uint32_t count, int drawMode)
{
    R_CORE_ASSERT(!indexBuffer, "Index buffer was already created for that geometry");
    indexBuffer = std::make_shared<IndexBuffer>(data, count, drawMode);
}

void RightEngine::Geometry::CreateVertexArray(const VertexBufferLayout& layout)
{
    R_CORE_ASSERT(!vertexArray, "Vertex array was already created for that geometry");
    R_CORE_ASSERT(vertexBuffer, "Attempt to create vertex array without vertex buffer declared!");
    vertexArray = std::make_shared<VertexArray>();
    vertexArray->AddBuffer(*vertexBuffer, layout);
}

const std::shared_ptr<VertexBuffer>& RightEngine::Geometry::GetVertexBuffer() const
{
    return vertexBuffer;
}

const std::shared_ptr<IndexBuffer>& RightEngine::Geometry::GetIndexBuffer() const
{
    return indexBuffer;
}

const std::shared_ptr<VertexArray>& Geometry::GetVertexArray() const
{
    return vertexArray;
}

