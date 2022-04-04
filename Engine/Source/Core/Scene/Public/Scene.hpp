#pragma once

#include "Camera.hpp"
#include "Components.hpp"
#include <entt.hpp>

namespace RightEngine
{
    class Entity;

    class Scene : public std::enable_shared_from_this<Scene>
    {
    public:
        virtual void OnUpdate();

        const std::shared_ptr<Camera>& GetCamera() const;
        void SetCamera(const std::shared_ptr<Camera>& camera);

        const std::shared_ptr<Entity>& GetRootNode() const;

        const entt::registry& GetRegistry() const;
        std::shared_ptr<Entity> CreateEntity();
        void DestroyEntity(const std::shared_ptr<Entity>& node);

        void UpdateNodeTransformRecursively(const std::shared_ptr<Entity>& node);

        static std::shared_ptr<Scene> Create();

    private:
        std::shared_ptr<Entity> rootNode;
        std::shared_ptr<Camera> camera;
        entt::registry registry;

    private:
        friend class Entity;
    };
}