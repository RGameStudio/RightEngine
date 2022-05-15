#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Scene.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

void Renderer::SubmitMesh(const std::shared_ptr<Shader>& shader,
                              const Mesh& mesh,
                              const glm::mat4& transform)
{
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", sceneData.viewProjectionMatrix);
    shader->SetUniformMat4f("u_Transform", transform);
    const auto& va = mesh.GetVertexArray();
    const auto& ib = va->GetIndexBuffer();
    const auto& vb = va->GetVertexBuffer();
    va->Bind();
    if (ib)
    {
        ib->Bind();
        RendererCommand::DrawIndexed(ib);
    }
    else
    {
        RendererCommand::Draw(vb);
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

GPU_API Renderer::GetAPI()
{
    return RendererAPI::GetAPI();
}
