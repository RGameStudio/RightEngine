#include "IndexBuffer.hpp"
#include <glad/glad.h>

void RightEngine::IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void RightEngine::IndexBuffer::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

RightEngine::IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &id);
}

RightEngine::IndexBuffer::IndexBuffer(const uint32_t *data, uint32_t count, int drawMode)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, drawMode);
}
