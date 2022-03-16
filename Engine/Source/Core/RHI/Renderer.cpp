#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Renderer.hpp"
#include "Core.h"
#include "LightNode.hpp"

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
    std::memset((void*)(&lightInfo), 0, sizeof(lightInfo));
}

void RightEngine::Renderer::Draw(const std::shared_ptr<Geometry>& geometry) const
{
    shader->Bind();
    const auto material = geometry->GetMaterial();
    const auto& materialData = material->GetMaterialData();
    const auto baseTexture = material->GetBaseTexture();
    if (baseTexture)
    {
        material->GetBaseTexture()->Bind();
        shader->SetUniform1i("baseTexture", 0);
        shader->SetUniform1i("hasBaseTexture", true);
    }
    else
    {
        shader->SetUniform1i("hasBaseTexture", false);
        shader->SetUniform4f("baseColor", materialData.baseColor);
    }

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
            lightInfo.ambientIntensity = node->GetIntensity();
            break;
        case LightNodeType::POINT_LIGHT:
            assert(false);
            break;
        default:
            R_CORE_ASSERT(false, "Unknown light type!")
            break;
    }
}

void Renderer::SaveLight() const
{
    shader->Bind();
    shader->SetUniform1i("hasAmbient", lightInfo.hasAmbient);
    shader->SetUniform1f("ambientStrength", lightInfo.ambientIntensity);
    shader->SetUniform3f("ambientColor", lightInfo.ambientColor);
}
