#pragma once

#include "Shader.hpp"

namespace RightEngine
{
    class BasicShader : public Shader
    {
    public:
        BasicShader();

        virtual void OnSetup(const std::shared_ptr<Scene>& scene) override;
        virtual void OnNodeDraw(const std::shared_ptr<SceneNode>& node) override;
        virtual void OnLightSave(LightInfo lightInfo) override;
    };
}