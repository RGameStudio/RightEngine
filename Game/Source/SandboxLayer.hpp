#pragma once

#include "Core.h"

class SandboxLayer : public RightEngine::Layer
{
public:
    SandboxLayer(): Layer("Sandbox") {}

    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;
private:
    std::shared_ptr<RightEngine::Scene> scene;
};
