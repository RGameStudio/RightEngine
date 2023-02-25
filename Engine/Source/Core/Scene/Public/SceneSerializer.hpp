#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace RightEngine
{
    struct AssetDependency
    {
        AssetType type;
        xg::Guid guid;
        std::string path;
    };

    struct MaterialAssetDependency : public AssetDependency
    {
        xg::Guid albedoGuid;
        xg::Guid normalGuid;
        xg::Guid roughnessGuid;
        xg::Guid metallicGuid;
        xg::Guid aoGuid;

        glm::vec3 albedo;
        float roughness;
        float metallic;
    };

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
        void DeserializeAssets(YAML::Node& node);
        void LoadDependencies(const std::vector<std::shared_ptr<AssetDependency>>& assetDependencies);

        std::shared_ptr<Scene> scene;
        std::unordered_set<xg::Guid> sceneAssets;
    };
}