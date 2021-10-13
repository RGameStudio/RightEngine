#include "VertexBuffer.hpp"
#include <Logger.hpp>

RightEngine::VertexBuffer::VertexBuffer(const void *data, uint32_t size, int drawMode): size(size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    if (drawMode == GL_STATIC_DRAW)
    {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    } else if (drawMode == GL_DYNAMIC_DRAW)
    {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    }

    R_CORE_TRACE("Created vertex buffer with id {0}", id);
}

RightEngine::VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

void RightEngine::VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void RightEngine::VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RightEngine::VertexBuffer::VertexBuffer()
{

}
