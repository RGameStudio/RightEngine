#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace RightEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& aScene);

        bool Serialize(const std::string& path);
        bool Deserialize(const std::string& path);

    private:
        void SerializeEntity(YAML::Emitter& output, const std::shared_ptr<Entity>& entity);
        void SerializeAssets(YAML::Emitter& output);
        void SaveMaterial(const AssetHandle& handle);

        std::shared_ptr<Scene> scene;
        std::unordered_set<xg::Guid> sceneAssets;
    };
}