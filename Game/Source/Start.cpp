#include "EntryPoint.hpp"
#include "Core.h"
#include "SandboxLayer.hpp"

static std::shared_ptr<SandboxLayer> layer;

void GameApplication::OnStart()
{
    layer = std::make_shared<SandboxLayer>();
    RightEngine::Application::Get().PushLayer(layer);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
