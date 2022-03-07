#pragma once

#include "SceneNode.hpp"

namespace RightEngine
{
    class Scene
    {
    public:
        Scene();

        virtual void Update();

        virtual void Draw(const std::shared_ptr<Shader>& shader);

    private:
        std::shared_ptr<SceneNode> rootNode;
    };
}