#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "Core.hpp"
#include "LightNode.hpp"
#include "Application.hpp"
#include <glm/ext/matrix_clip_space.hpp>

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

void RightEngine::Renderer::Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Geometry>& geometry, const glm::mat4& transform) const
{
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", sceneData.viewProjectionMatrix);
    shader->SetUniformMat4f("u_Transform", transform);
    shader->SetMaterialUniforms(geometry->GetMaterial());
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

void Renderer::SetClearColor(const glm::vec4& color) const
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear(uint32_t clearBits) const
{
    glClear(clearBits);
}

void Renderer::BeginScene(const std::shared_ptr<Scene>& scene)
{
    // TODO: Get FOV from camera
    const auto projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                   static_cast<float>(window->GetWidth()) /
                                                   static_cast<float>(window->GetHeight()),
                                                   0.1f,
                                                   300.0f);
    const auto viewMatrix = scene->GetCamera()->GetViewMatrix();
    sceneData.viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Renderer::EndScene()
{
}
