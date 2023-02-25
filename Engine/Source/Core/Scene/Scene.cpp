#include "Scene.hpp"
#include "Assert.hpp"
#include "Entity.hpp"

using namespace RightEngine;

std::shared_ptr<Scene> Scene::Create()
{
    auto sceneRawPtr = new Scene();

    std::shared_ptr<Scene> scene;
    scene.reset(sceneRawPtr);
    scene->rootNode = scene->CreateEntity();

    return scene;
}

void RightEngine::Scene::OnUpdate(float deltaTime)
{
    UpdateNodeTransformRecursively(rootNode);
}

const std::shared_ptr<Entity>& Scene::GetRootNode() const
{
    return rootNode;
}

std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name, bool addToRoot)
{
    std::shared_ptr<Entity> entity = std::make_shared<Entity>(registry.create(), shared_from_this());
    entity->AddComponent<TransformComponent>();
    entity->AddComponent<TagComponent>(name);

    if (addToRoot)
    {
        rootNode->AddChild(entity);
    }

    return entity;
}

void Scene::DestroyEntity(const std::shared_ptr<Entity>& node)
{
    registry.destroy(node->entityHandle);
}

void Scene::UpdateNodeTransformRecursively(const std::shared_ptr<Entity>& node)
{
    auto& transform = node->GetComponent<TransformComponent>();
    const auto parentPtr = node->parent.lock();
    if (parentPtr)
    {
        transform.RecalculateTransform(parentPtr->GetComponent<TransformComponent>());
    }
    else
    {
        transform.RecalculateTransform();
    }

    for (const auto& child: node->children)
    {
        UpdateNodeTransformRecursively(child);
    }
}

entt::registry& Scene::GetRegistry()
{
    return registry;
}

std::shared_ptr<Entity> Scene::CreateEntityWithGuid(const std::string& name, const xg::Guid& guid, bool addToRoot)
{
    auto entity = CreateEntity(name, addToRoot);
    auto& tag = entity->GetComponent<TagComponent>();
    tag.guid = guid;

    return entity;
}
