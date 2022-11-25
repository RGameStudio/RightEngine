#pragma once

#include "Scene.hpp"
#include "Entity.hpp"
#include "AssetBase.hpp"
#include <memory>

namespace RightEngine
{
    class PropertyPanel
    {
    public:
        PropertyPanel() = default;
        ~PropertyPanel() = default;

        PropertyPanel(const std::shared_ptr<Scene>& aScene);

        void SetScene(const std::shared_ptr<Scene>& aScene);
        void SetSelectedEntity(const std::shared_ptr<Entity>& entity);

        void OnImGuiRender();

    private:
        std::shared_ptr<Scene> scene;
        std::shared_ptr<Entity> selectedEntity;
        std::unordered_map<std::string, AssetHandle> environmentMaps;
        std::unordered_map<std::string, AssetHandle> meshes;
    };
}