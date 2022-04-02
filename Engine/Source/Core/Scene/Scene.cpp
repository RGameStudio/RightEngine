#include "Scene.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"

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

void RightEngine::Scene::SetCamera(const std::shared_ptr<Camera>& camera)
{
    R_CORE_ASSERT(!this->camera, "Camera was already set!");
    this->camera = camera;
}

const std::shared_ptr<Camera>& RightEngine::Scene::GetCamera() const
{
    return camera;
}

const std::shared_ptr<SceneNode>& Scene::GetRootNode() const
{
    return rootNode;
}

std::shared_ptr<SceneNode> Scene::CreateNode()
{
    std::shared_ptr<SceneNode> node = std::make_shared<SceneNode>(registry.create(), shared_from_this());
    return node;
}

void Scene::DestroyNode(const std::shared_ptr<SceneNode>& node)
{

}
