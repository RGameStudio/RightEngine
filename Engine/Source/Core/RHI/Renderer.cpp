#include "Renderer.hpp"

void RightEngine::Renderer::Draw(const RightEngine::VertexArray &va, const RightEngine::IndexBuffer &ib)
{
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RightEngine::Renderer::Draw(const RightEngine::VertexArray &va, const RightEngine::VertexBuffer &vb)
{
    va.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vb.GetSize());
}
