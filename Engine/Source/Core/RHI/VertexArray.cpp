#include "VertexArray.hpp"

RightEngine::VertexArray::VertexArray(): id(0)
{
    glGenVertexArrays(1, &id);
}

RightEngine::VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

void RightEngine::VertexArray::Bind() const
{
    glBindVertexArray(id);
}

void RightEngine::VertexArray::UnBind() const
{
    glBindVertexArray(0);
}

void RightEngine::VertexArray::AddBuffer(const RightEngine::VertexBuffer& vb, const RightEngine::VertexBufferLayout& layout) const
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}
