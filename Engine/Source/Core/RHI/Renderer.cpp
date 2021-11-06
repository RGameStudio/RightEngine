#include "Renderer.hpp"
#include <Logger.hpp>

void RightEngine::Renderer::Draw(const RightEngine::VertexArray &va, const RightEngine::IndexBuffer &ib)
{
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
    va.UnBind();
    ib.UnBind();
}

void RightEngine::Renderer::Draw(const RightEngine::VertexArray &va, const RightEngine::VertexBuffer &vb)
{
    va.Bind();
    vb.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vb.GetSize());
    va.UnBind();
    vb.UnBind();
}

void RightEngine::Renderer::Draw(const RightEngine::VertexArray &va, const RightEngine::IndexBuffer &ib,
                                 const RightEngine::Shader &shader)
{
    shader.Bind();
    Draw(va, ib);
    shader.UnBind();
}
