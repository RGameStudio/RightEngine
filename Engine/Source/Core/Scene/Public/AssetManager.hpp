#pragma once

#include "Texture.hpp"
#include "TextureLoader.hpp"
#include "MeshLoader.hpp"
#include "EnvironmentMapLoader.hpp"
#include "Shader.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace RightEngine
{
    struct LoaderOptions
    {
        bool flipTextureVertically{ false };
        bool convertTextureFormat{ false };
    };

    // TODO: Implement AssetManager in a new way
    // Use AssetHandle with UUID to refer to specific asset
    // Use assets factories to use asset-specific loading options with ease

    class AssetManager
    {
    public:
        static AssetManager& Get();

        template<class T>
        std::shared_ptr<T> GetAsset(const std::string& id)
        {
            R_CORE_ASSERT(!id.empty(), "");
            R_CORE_ASSERT(static_cast<bool>(std::is_base_of<AssetBase, T>::value), "");
            auto assetIt = assetCache.find(id);
            if (assetIt == assetCache.end())
            {
                return nullptr;
            }
            auto ptr = std::dynamic_pointer_cast<T>(assetIt->second);
            R_CORE_ASSERT(ptr != nullptr, "");
            return ptr;
        }

        template<class T>
        std::shared_ptr<T> LoadAsset(const std::string& path, const std::string& id, const LoaderOptions& options)
        {
            R_CORE_ASSERT(!id.empty(), "");
            R_CORE_ASSERT(false, "");
            return nullptr;
        }

        void RemoveAsset(const std::string& id)
        {
            assetCache.erase(id);
        }

        AssetManager(const AssetManager& other) = delete;
        AssetManager& operator=(const AssetManager& other) = delete;
        AssetManager(AssetManager&& other) = delete;
        AssetManager& operator=(AssetManager&& other) = delete;

    private:
        std::unordered_map<std::string, std::shared_ptr<AssetBase>> assetCache;

    private:
        AssetManager() = default;
        ~AssetManager() = default;

        template<class T>
        void CacheAsset(const std::shared_ptr<T>& ptr, const std::string& id, AssetType type)
        {
            R_CORE_ASSERT(static_cast<bool>(std::is_base_of_v<AssetBase, T>), "");
            auto basePtr = std::dynamic_pointer_cast<AssetBase>(ptr);
            R_CORE_ASSERT(basePtr != nullptr, "");
            basePtr->id = id;
            basePtr->type = type;
            assetCache[id] = basePtr;
        }
    };

    template<>
    inline std::shared_ptr<Texture> AssetManager::LoadAsset(const std::string& path, const std::string& id, const LoaderOptions& options)
    {
        auto asset = GetAsset<Texture>(id);
        if (asset)
        {
            return asset;
        }

        TextureLoader loader;
//        TextureDescriptor
//        auto texture = loader.CreateTexture(path, options.flipTextureVertically);
//        CacheAsset(texture, id, AssetType::IMAGE);
//        return texture;
    }

    template<>
    inline std::shared_ptr<MeshNode> AssetManager::LoadAsset(const std::string& path, const std::string& id, const LoaderOptions& options)
    {
        auto asset = GetAsset<MeshNode>(id);
        if (asset)
        {
            return asset;
        }

        MeshLoader loader;
        auto mesh = loader.Load(path);
        CacheAsset(mesh, id, AssetType::MESH);
        return mesh;
    }

    template<>
    inline std::shared_ptr<EnvironmentContext> AssetManager::LoadAsset(const std::string& path, const std::string& id, const LoaderOptions& options)
    {
        auto asset = GetAsset<EnvironmentContext>(id);
        if (asset)
        {
            return asset;
        }

        EnvironmentMapLoader loader;
        loader.Load(path, options.flipTextureVertically);
        auto env = loader.GetEnvironmentContext();
        CacheAsset(env, id, AssetType::ENVIRONMENT_MAP);
        return env;
    }

    template<>
    inline std::shared_ptr<Shader> AssetManager::LoadAsset(const std::string& path, const std::string& id, const LoaderOptions& options)
    {
        // TODO: Implement shader reflection here with this - https://github.com/KhronosGroup/SPIRV-Reflect
        R_CORE_ASSERT(false, "");
        auto asset = GetAsset<Shader>(id);
        if (asset)
        {
            return asset;
        }

//        auto shader = Shader::Create(path + ".vert", path + ".frag");
//        CacheAsset(shader, id, AssetType::SHADER);
        return nullptr;
    }
}
