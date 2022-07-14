#pragma once

#include "Components.hpp"
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>

namespace RightEngine
{
    struct MeshNode
    {
        std::vector<std::shared_ptr<MeshComponent>> meshes;
        std::vector<std::shared_ptr<MeshNode>> children;
    };

    class MeshLoader
    {
    public:
        MeshLoader() = default;
        ~MeshLoader() = default;

        std::shared_ptr<MeshNode> Load(const std::string& path);

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene, std::shared_ptr<MeshNode>& meshNode);
        std::shared_ptr<MeshComponent> ProcessMesh(const aiMesh* mesh, const aiScene* scene);
        std::vector<std::shared_ptr<Texture>> LoadTextures(const aiMaterial* mat, aiTextureType type);

    private:
        std::string meshDir;
        std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
    };
}