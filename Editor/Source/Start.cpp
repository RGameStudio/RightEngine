#include "EntryPoint.hpp"
#include "EditorLayer.hpp"
#include "Service/SelectionService.hpp"

static std::shared_ptr<editor::EditorLayer> layer;

void GameApplication::OnStart()
{
    auto& app = RightEngine::Instance();

    app.RegisterService<editor::SelectionService>();

    layer = std::make_shared<editor::EditorLayer>();
    app.PushLayer(layer);

}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}

std::string G_ASSET_DIR = ASSETS_DIR;
std::string G_CONFIG_DIR = CONFIG_DIR;
