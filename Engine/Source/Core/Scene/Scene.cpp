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
    camera->OnUpdate(deltaTime);
    UpdateNodeTransformRecursively(rootNode);
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

const std::shared_ptr<Entity>& Scene::GetRootNode() const
{
    return rootNode;
}

std::shared_ptr<Entity> Scene::CreateEntity()
{
    std::shared_ptr<Entity> entity = std::make_shared<Entity>(registry.create(), shared_from_this());
    entity->AddComponent<TransformComponent>();
    return entity;
}

void Scene::DestroyEntity(const std::shared_ptr<Entity>& node)
{
    // TODO: Implement
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

const entt::registry& Scene::GetRegistry() const
{
    return registry;
}
