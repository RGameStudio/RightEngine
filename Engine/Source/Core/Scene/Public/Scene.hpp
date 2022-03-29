#pragma once

#include "Camera.hpp"
#include "SceneNode.hpp"

namespace RightEngine
{
    class Scene : public std::enable_shared_from_this<Scene>
    {
    public:
        Scene();

        virtual void OnUpdate();
        virtual void OnRender(const std::shared_ptr<Shader>& shader);

        const std::shared_ptr<FPSCamera>& GetCamera() const;
        void SetCamera(const std::shared_ptr<FPSCamera>& camera);

        const std::shared_ptr<SceneNode>& GetRootNode() const;

    private:
        std::shared_ptr<SceneNode> rootNode;
        std::shared_ptr<FPSCamera> camera;
    };
}