#pragma once

#include "Components.hpp"
#include <entt.hpp>

namespace RightEngine
{
    class Entity;

    class Scene : public std::enable_shared_from_this<Scene>
    {
    public:
        virtual void OnUpdate(float deltaTime);

        const std::shared_ptr<Entity>& GetRootNode() const;

        entt::registry& GetRegistry();
        std::shared_ptr<Entity> CreateEntity(const std::string& name = "New entity", bool addToRoot = false);
        void DestroyEntity(const std::shared_ptr<Entity>& node);

        void UpdateNodeTransformRecursively(const std::shared_ptr<Entity>& node);

        static std::shared_ptr<Scene> Create();

    private:
        std::shared_ptr<Entity> rootNode;
        entt::registry registry;

    private:
        friend class Entity;
    };
}