#pragma once

#include "AssetBase.hpp"
#include "Components.hpp"
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>

namespace RightEngine
{
    struct MeshNode : public AssetBase
    {
        ASSET_BASE()

        std::vector<std::shared_ptr<MeshComponent>> meshes;
        std::vector<std::shared_ptr<MeshNode>> children;
    };

    class MeshLoader : public AssetLoader
    {
    public:
        MeshLoader() = default;
        ~MeshLoader() = default;

        AssetHandle Load(const std::string& path);

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene, std::shared_ptr<MeshNode>& meshNode);
        std::shared_ptr<MeshComponent> ProcessMesh(const aiMesh* mesh, const aiScene* scene);
        std::vector<std::shared_ptr<Texture>> LoadTextures(const aiMaterial* mat, aiTextureType type);

    private:
        std::string meshDir;
        std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
    };
}