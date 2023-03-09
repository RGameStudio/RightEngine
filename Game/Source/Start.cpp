#include "EntryPoint.hpp"
#include "GameLayer.hpp"

static std::shared_ptr<GameLayer> layer;

void GameApplication::OnStart()
{
    layer = std::make_shared<GameLayer>();
    RightEngine::Application::Get().PushLayer(layer);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}

std::string G_ASSET_DIR = ASSETS_DIR;
