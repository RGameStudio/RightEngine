#include "OpenGLRendererAPI.hpp"
#include "Renderer.hpp"

RightEngine::OpenGLRendererAPI::OpenGLRendererAPI()
{
    Init();
}

void RightEngine::OpenGLRendererAPI::Init()
{
    // TODO: Move OpenGL debug code here
}

void RightEngine::OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RightEngine::OpenGLRendererAPI::Clear(uint32_t clearBits)
{
    glClear(clearBits);
}

void RightEngine::OpenGLRendererAPI::SetViewport(const Viewport& viewport)
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

RightEngine::Viewport RightEngine::OpenGLRendererAPI::GetViewport()
{
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return { viewport[0], viewport[1], viewport[2], viewport[3] };
}

void RightEngine::OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<IndexBuffer>& ib)
{
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RightEngine::OpenGLRendererAPI::Draw(const std::shared_ptr<VertexBuffer>& vb)
{
    glDrawArrays(GL_TRIANGLES, 0, vb->GetSize());
}

void RightEngine::OpenGLRendererAPI::Configure(const RightEngine::RendererSettings& settings)
{
    if (settings.hasDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}
