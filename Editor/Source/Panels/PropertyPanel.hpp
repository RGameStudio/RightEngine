#pragma once

#include "Scene.hpp"
#include "Entity.hpp"
#include "AssetBase.hpp"
#include <imgui.h>
#include <memory>

namespace editor
{
    class PropertyPanel
    {
    public:
        PropertyPanel() = default;
        ~PropertyPanel() = default;

        PropertyPanel(const std::shared_ptr<RightEngine::Scene>& aScene);

        void SetScene(const std::shared_ptr<RightEngine::Scene>& aScene);
        void SetSelectedEntity(const std::shared_ptr<RightEngine::Entity>& entity);

        void OnImGuiRender();

        void Init();

    private:
        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName)
        {
            if (!selectedEntity->HasComponent<T>())
            {
                if (ImGui::MenuItem(entryName.c_str()))
                {
                    selectedEntity->AddComponent<T>();
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        std::shared_ptr<RightEngine::Scene> scene;
        std::shared_ptr<RightEngine::Entity> selectedEntity;
        std::unordered_map<std::string, RightEngine::AssetHandle> environmentMaps;
        std::unordered_map<std::string, RightEngine::AssetHandle> meshes;
        std::unordered_map<std::string, RightEngine::AssetHandle> textures;
    };
}