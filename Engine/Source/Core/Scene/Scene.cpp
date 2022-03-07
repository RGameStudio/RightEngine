#include "Scene.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

RightEngine::Scene::Scene()
{
    rootNode = std::make_shared<SceneNode>();
}

void RightEngine::Scene::OnUpdate()
{
    camera->OnUpdate();
    const auto& renderer = Renderer::Get();
    const auto window = renderer.GetWindow();
    const auto shader = renderer.GetShader();
    const auto projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                        static_cast<float>(window->GetWidth()) /
                                                        static_cast<float>(window->GetHeight()), 0.1f, 300.0f);
    shader->Bind();
    shader->SetUniformMat4f("projection", projectionMatrix);
    shader->SetUniformMat4f("view", camera->GetViewMatrix());
    std::vector<std::shared_ptr<SceneNode>> nodes = rootNode->GetAllChildren();
    for (const auto& node: nodes)
    {
        node->OnUpdate();
        shader->SetUniformMat4f("model", node->GetModelMatrix());
        node->Draw();
    }
}

void RightEngine::Scene::SetCamera(const std::shared_ptr<FPSCamera>& camera)
{
    R_CORE_ASSERT(!this->camera, "Camera was already set!");
    this->camera = camera;
}

const std::shared_ptr<FPSCamera>& RightEngine::Scene::GetCamera() const
{
    return camera;
}

const std::shared_ptr<SceneNode>& Scene::GetRootNode() const
{
    return rootNode;
}
