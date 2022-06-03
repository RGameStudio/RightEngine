#include "OpenGLRendererAPI.hpp"
#include "Renderer.hpp"

using namespace RightEngine;

namespace
{
    int DepthTestEnumToGLEnum(DepthTestMode mode)
    {
        switch (mode)
        {
            case DepthTestMode::LESS:
                return GL_LESS;
            case DepthTestMode::LEQUAL:
                return GL_LEQUAL;
            default:
                R_CORE_ASSERT(false, "");
                return -1;
        }
    }
}

OpenGLRendererAPI::OpenGLRendererAPI()
{
    Init();
}

void OpenGLRendererAPI::Init()
{
    // TODO: Move OpenGL debug code here
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear(uint32_t clearBits)
{
    glClear(clearBits);
}

void OpenGLRendererAPI::SetViewport(const Viewport& viewport)
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

Viewport OpenGLRendererAPI::GetViewport()
{
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return { viewport[0], viewport[1], viewport[2], viewport[3] };
}

void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<IndexBuffer>& ib)
{
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::Draw(const std::shared_ptr<VertexBuffer>& vb)
{
    glDrawArrays(GL_TRIANGLES, 0, vb->GetSize());
}

void OpenGLRendererAPI::Configure(const RendererSettings& settings)
{
    if (settings.hasDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthFunc(DepthTestEnumToGLEnum(settings.depthTestMode));
}
