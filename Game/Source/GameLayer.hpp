#pragma once

#include "Core.hpp"
#include "Texture.hpp"

class GameLayer : public RightEngine::Layer
{
public:
    GameLayer(): Layer("Game") {}

    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;
    virtual void OnImGuiRender();
    bool OnEvent(const Event& event);

    void LoadDefaultScene();
    
private:
    std::shared_ptr<RightEngine::Scene> scene;
    std::shared_ptr<RightEngine::Scene> newScene;

    const std::string DEFAULT_SCENE_PATH = G_ASSET_DIR + "/Scenes/Scene.yaml";
};
