#pragma once

#include "Camera.hpp"
#include "SceneNode.hpp"
#include <entt.hpp>

namespace RightEngine
{
    class Scene : public std::enable_shared_from_this<Scene>
    {
    public:
        Scene();

        virtual void OnUpdate();

        const std::shared_ptr<Camera>& GetCamera() const;
        void SetCamera(const std::shared_ptr<Camera>& camera);

        const std::shared_ptr<SceneNode>& GetRootNode() const;

        std::shared_ptr<SceneNode> CreateNode();
        void DestroyNode(const std::shared_ptr<SceneNode>& node);

    private:
        std::shared_ptr<SceneNode> rootNode;
        std::shared_ptr<Camera> camera;
        entt::registry registry;

        friend class SceneNode;
    };
}