#include "MeshLoader.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "String.hpp"
#include "TextureLoader.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace RightEngine;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 biTangent;
    };

    std::shared_ptr<MeshComponent> BuildMesh(const std::vector<Vertex>& vertices,
                                             const std::vector<uint32_t>& indexes,
                                             const std::shared_ptr<Material>& material)
    {
        R_CORE_ASSERT(!vertices.empty() && material != nullptr, "");
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        layout.Push<float>(3);
        layout.Push<float>(3);

        auto mesh = std::make_shared<MeshComponent>();
        auto vertexArray = std::make_shared<VertexArray>();
        vertexArray->AddBuffer(std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex)), layout);
        if (!indexes.empty())
        {
            vertexArray->AddBuffer(std::make_shared<IndexBuffer>(indexes.data(), indexes.size()));
        }

        mesh->SetVertexArray(vertexArray);
        mesh->SetMaterial(material);

        return mesh;
    }
}

std::shared_ptr<MeshNode> MeshLoader::Load(const std::string& aPath)
{
    const size_t lastDelimIndex = aPath.find_last_of('/');
    std::string meshName = aPath.substr(lastDelimIndex, aPath.size() - 1 - lastDelimIndex);
    meshDir = aPath.substr(0, lastDelimIndex);
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(Path::ConvertEnginePathToOSPath(aPath),
                                         aiProcess_Triangulate
                                         | aiProcess_GenSmoothNormals
                                         | aiProcess_FlipUVs
                                         | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        R_CORE_ERROR("ASSIMP ERROR: {0}", importer.GetErrorString());
        return nullptr;
    }

    auto meshTree = std::make_shared<MeshNode>();
    ProcessNode(scene->mRootNode, scene, meshTree);

    return meshTree;
}

void MeshLoader::ProcessNode(const aiNode* node, const aiScene* scene, std::shared_ptr<MeshNode>& meshNode)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshNode->meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        auto newNode = std::make_shared<MeshNode>();
        meshNode->children.push_back(newNode);
        ProcessNode(node->mChildren[i], scene, meshNode);
    }
}

std::shared_ptr<MeshComponent> MeshLoader::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indexes;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents())
        {
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;

            glm::vec3 biTangent;
            biTangent.x = mesh->mBitangents[i].x;
            biTangent.y = mesh->mBitangents[i].y;
            biTangent.z = mesh->mBitangents[i].z;

            vertex.tangent = tangent;
            vertex.biTangent = biTangent;
        }
        else
        {
            // TODO: Implement manual tangent and biTangent calculation
            R_CORE_ASSERT(false, "");
        }

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indexes.push_back(face.mIndices[j]);
        }
    }

    std::vector<std::shared_ptr<Texture>> albedo;
    std::vector<std::shared_ptr<Texture>> normal;
    std::vector<std::shared_ptr<Texture>> specular;
    std::vector<std::shared_ptr<Texture>> roughness;
#if 0
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        albedo = LoadTextures(material, aiTextureType_DIFFUSE);
        normal = LoadTextures(material, aiTextureType_NORMALS);
        specular = LoadTextures(material, aiTextureType_SPECULAR);
        if (specular.empty())
        {
            specular = LoadTextures(material, aiTextureType_METALNESS);
        }
        roughness = LoadTextures(material, aiTextureType_DIFFUSE_ROUGHNESS);
        if (roughness.empty())
        {
            roughness = LoadTextures(material, aiTextureType_SHININESS);
        }
    }
#endif

    auto material = std::make_shared<Material>();
    if (!albedo.empty())
    {
        material->textureData.albedo = albedo.front();
    }
    if (!normal.empty())
    {
        material->textureData.normal = normal.front();
    }
    if (!specular.empty())
    {
        material->textureData.metallic = specular.front();
    }
    if (!roughness.empty())
    {
        material->textureData.roughness = roughness.front();
    }
    auto builtMesh = BuildMesh(vertices, indexes, material);

    return builtMesh;
}

// Current supported path convention for model's textures is this:
//          model.file
//          textures_dir/<all_textures>

std::vector<std::shared_ptr<Texture>> MeshLoader::LoadTextures(const aiMaterial* mat, aiTextureType type)
{
    std::vector<std::shared_ptr<Texture>> textures;
    TextureLoader textureLoader;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // TODO: Add / delimiter support
        auto splittedPath = String::Split(std::string(str.C_Str()), "\\");

        std::string texName = meshDir + '/' + *(splittedPath.end() - 2) + '/' + *(splittedPath.end() - 1);
        if (loadedTextures.find(texName) == loadedTextures.end())
        {
            auto texture = textureLoader.CreateTexture(texName);
            textures.push_back(texture);
            loadedTextures[texName] = texture;
        }
        else
        {
            textures.push_back(loadedTextures[texName]);
        }
    }
    return textures;
}
