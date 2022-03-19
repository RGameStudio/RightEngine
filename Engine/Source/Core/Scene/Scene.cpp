#include "Scene.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include "LightNode.hpp"
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
    SetupLights(nodes);
    for (const auto& node: nodes)
    {
        node->OnUpdate();
        shader->SetUniformMat4f("model", node->GetWorldModelMatrix());
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

void Scene::SetupLights(const std::vector<std::shared_ptr<SceneNode>>& nodes)
{
    bool hasLight = false;
    for (const auto& node: nodes)
    {
        if (node->GetBaseType() == NodeType::LIGHT)
        {
            const auto lightNode = std::static_pointer_cast<LightNode>(node);
            Renderer::Get().SetLight(lightNode);
            hasLight = true;
        }
    }

    if (!hasLight)
    {
        R_CORE_WARN("Scene doesn't have any lights!");
    }

    Renderer::Get().SaveLight();
}
