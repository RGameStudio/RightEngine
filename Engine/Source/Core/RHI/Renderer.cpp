#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Scene.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

void Renderer::SubmitGeometry(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Geometry>& geometry,
                              const glm::mat4& transform)
{
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", sceneData.viewProjectionMatrix);
    shader->SetUniformMat4f("u_Transform", transform);
    geometry->GetVertexArray()->Bind();
    geometry->GetVertexBuffer()->Bind();
    if (geometry->GetIndexBuffer())
    {
        geometry->GetIndexBuffer()->Bind();
        RendererCommand::DrawIndexed(geometry->GetIndexBuffer());
    }
    else
    {
        RendererCommand::Draw(geometry->GetVertexBuffer());
    }
}

void Renderer::BeginScene(const std::shared_ptr<Scene>& scene)
{
// TODO: Get FOV from camera
// TODO: Get aspect ratio from camera
    const auto projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                   16.0f/ 9.0f,
                                                   0.1f,
                                                   300.0f);
    const auto viewMatrix = scene->GetCamera()->GetViewMatrix();
    sceneData.viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Renderer::EndScene()
{}

void Renderer::Configure()
{
    RendererCommand::Configure(settings);
}

RendererSettings& Renderer::GetSettings()
{
    return settings;
}
