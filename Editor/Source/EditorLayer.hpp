#pragma once

#include "Core.hpp"
#include "Texture.hpp"
#include "Panels/ContentBrowserPanel.hpp"

namespace editor
{
    class EditorLayer : public RightEngine::Layer
    {
    public:
        EditorLayer() : Layer("Game") {}

        virtual void OnAttach() override;
        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender();
        bool OnEvent(const Event& event);

        void LoadDefaultScene();
        std::shared_ptr<RightEngine::Scene> LoadScene(const fs::path& path);

        void NewScene();
        void OpenScene(const fs::path& path);

    private:
        void Scene(const std::shared_ptr<RightEngine::Scene>& scene);
        std::shared_ptr<RightEngine::Scene> m_scene;
        std::shared_ptr<RightEngine::Scene> m_newScene;

        ContentBrowserPanel m_contentBrowser;

        const std::string C_DEFAULT_SCENE_PATH = G_ASSET_DIR + "/Scenes/Scene.scene";
    };
}
