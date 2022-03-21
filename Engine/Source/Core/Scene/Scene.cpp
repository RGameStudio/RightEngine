#include "Scene.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include "LightNode.hpp"

using namespace RightEngine;

RightEngine::Scene::Scene()
{
    rootNode = std::make_shared<SceneNode>();
}

void RightEngine::Scene::OnUpdate()
{
    camera->OnUpdate();
    std::vector<std::shared_ptr<SceneNode>> nodes = rootNode->GetAllChildren();
    SetupLights(nodes);
    auto& renderer = Renderer::Get();
    renderer.SetupDraw(shared_from_this());
    for (const auto& node: nodes)
    {
        node->OnUpdate();
        renderer.Draw(node);
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
