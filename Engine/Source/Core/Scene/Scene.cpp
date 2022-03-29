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
    for (const auto& node: nodes)
    {
        node->OnUpdate();
    }
}

void Scene::OnRender(const std::shared_ptr<Shader>& shader)
{
    std::vector<std::shared_ptr<SceneNode>> nodes = rootNode->GetAllChildren();
    for (const auto& node: nodes)
    {
        node->OnRender(shader);
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
