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

    // TODO: We should come up with more general approach for resources storing.
    // Maybe put this to a global header file in engine?
    const std::string ASSETS_DIRECTORY = ASSETS_DIR;
    const std::string DEFAULT_SCENE_PATH = ASSETS_DIRECTORY + "/Scenes/Scene.yaml";
};
