#include "Scene.hpp"

RightEngine::Scene::Scene()
{
    rootNode = std::make_shared<SceneNode>();
}

void RightEngine::Scene::Update()
{

}

void RightEngine::Scene::Draw(const std::shared_ptr<Shader>& shader)
{
    std::vector<std::shared_ptr<SceneNode>> nodesToDraw = rootNode->GetAllChildren();
    for (const auto& node: nodesToDraw)
    {
        node->Draw(shader);
    }
}
