#pragma once

#include "Core.hpp"

class SandboxLayer : public RightEngine::Layer
{
public:
    SandboxLayer(): Layer("Sandbox") {}

    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;
private:
    std::shared_ptr<RightEngine::Scene> scene;
    std::shared_ptr<RightEngine::Shader> shader;
    std::shared_ptr<RightEngine::Renderer> renderer;
};
