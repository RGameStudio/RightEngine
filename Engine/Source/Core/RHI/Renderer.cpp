#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "Core.hpp"
#include "LightNode.hpp"
#include "Application.hpp"

using namespace RightEngine;

void RightEngine::Renderer::Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib) const
{
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RightEngine::Renderer::Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<VertexBuffer>& vb) const
{
    glDrawArrays(GL_TRIANGLES, 0, vb->GetSize());
}

RightEngine::Renderer& RightEngine::Renderer::Get()
{
    static Renderer renderer;
    return renderer;
}

void RightEngine::Renderer::SetWindow(Window* _window)
{
    this->window = _window;
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeHandle()));

    R_CORE_ASSERT(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Can't load glad!");
}

RightEngine::Window* RightEngine::Renderer::GetWindow() const
{
    return window;
}

void RightEngine::Renderer::Clear() const
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    std::memset((void*) (&lightInfo), 0, sizeof(lightInfo));
}

void RightEngine::Renderer::Draw(const std::shared_ptr<Geometry>& geometry) const
{
    geometry->GetVertexArray()->Bind();
    geometry->GetVertexBuffer()->Bind();
    if (geometry->GetIndexBuffer())
    {
        geometry->GetIndexBuffer()->Bind();
        Draw(geometry->GetVertexArray(), geometry->GetIndexBuffer());
    }
    else
    {
        Draw(geometry->GetVertexArray(), geometry->GetVertexBuffer());
    }
}

void Renderer::SetShader(const std::shared_ptr<Shader>& shader)
{
    this->shader = shader;
}

const std::shared_ptr<Shader>& Renderer::GetShader() const
{
    return shader;
}

void Renderer::HasDepthTest(bool mode)
{
    if (mode)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::SetLight(const std::shared_ptr<LightNode>& node)
{
    switch (node->GetType())
    {
        case LightNodeType::AMBIENT:
            lightInfo.hasAmbient = true;
            lightInfo.ambientColor = node->GetColor();
            break;
        case LightNodeType::POINT_LIGHT:
            lightInfo.pointLightPosition[lightInfo.pointLightAmount] = node->GetWorldPosition();
            lightInfo.pointLightColor[lightInfo.pointLightAmount] = node->GetColor();
            lightInfo.pointLightAmount += 1;
            break;
        default:
        R_CORE_ASSERT(false, "Unknown light type!")
            break;
    }
}

void Renderer::SaveLight() const
{
    shader->OnLightSave(lightInfo);
}

void Renderer::SetupDraw(const std::shared_ptr<Scene>& scene)
{
    shader->OnSetup(scene);
}

void Renderer::Draw(const std::shared_ptr<SceneNode>& node) const
{
    shader->OnNodeDraw(node);
    const auto& geometry = node->GetGeometry();
    if (geometry)
    {
        Draw(geometry);
    }
}
