#include "VertexArray.hpp"

#include "Assert.hpp"

using namespace RightEngine;

VertexArray::VertexArray() : id(0)
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind() const
{
    glBindVertexArray(id);
    if (indexBuffer)
    {
        indexBuffer->Bind();
    }
    vertexBuffer->Bind();
}

void VertexArray::UnBind() const
{
    glBindVertexArray(0);
    if (indexBuffer)
    {
        indexBuffer->UnBind();
    }
    vertexBuffer->UnBind();
}

void VertexArray::AddBuffer(const std::shared_ptr<IndexBuffer>& ib)
{
    R_CORE_ASSERT(ib, "In index buffer is null!");
    R_CORE_ASSERT(!indexBuffer, "Index buffer was already set!");
    indexBuffer = ib;
}

void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout)
{
    R_CORE_ASSERT(vb, "In vertex buffer is null!");
    R_CORE_ASSERT(!vertexBuffer, "Vertex buffer was already set!");
    vertexBuffer = vb;
    Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
//        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(),
//                              (const void*) offset);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    UnBind();
}

const std::shared_ptr<VertexBuffer>& VertexArray::GetVertexBuffer() const
{
    return vertexBuffer;
}

const std::shared_ptr<IndexBuffer>& VertexArray::GetIndexBuffer() const
{
    return indexBuffer;
}
