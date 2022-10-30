#pragma once

#include "Texture.hpp"
#include "TextureLoader.hpp"
#include "MeshLoader.hpp"
#include "EnvironmentMapLoader.hpp"
#include "Shader.hpp"
#include "AssetLoader.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>

namespace RightEngine
{
    class AssetManager
    {
    public:
        static AssetManager& Get();

        template<class T>
        void RegisterLoader(const std::shared_ptr<AssetLoader>& loader)
        {
            auto typeIndex = std::type_index(typeid(T));
            auto loaderIt = loaders.find(typeIndex);
            if (loaderIt == loaders.end())
            {
                loaders[typeIndex] = loader;
                loader->OnRegister(this);
            }
            R_CORE_ASSERT(false, "")
        }

        AssetManager(const AssetManager& other) = delete;
        AssetManager& operator=(const AssetManager& other) = delete;
        AssetManager(AssetManager&& other) = delete;
        AssetManager& operator=(AssetManager&& other) = delete;

    private:
        std::unordered_map<xg::Guid, std::shared_ptr<AssetBase>> assetCache;
        std::unordered_map<std::type_index, std::shared_ptr<AssetLoader>> loaders;

        AssetManager() = default;
        ~AssetManager() = default;

        template<class T>
        AssetHandle CacheAsset(const std::shared_ptr<T>& ptr, AssetType type)
        {
            R_CORE_ASSERT(static_cast<bool>(std::is_base_of_v<AssetBase, T>), "");
            auto basePtr = std::dynamic_pointer_cast<AssetBase>(ptr);
            R_CORE_ASSERT(basePtr != nullptr, "");
            basePtr->guid = xg::newGuid();
            basePtr->type = type;
            assetCache[basePtr->guid] = basePtr;
            return { basePtr->guid };
        }

        template<class T>
        std::shared_ptr<T> GetAsset(const AssetHandle& assetHandle)
        {
            R_CORE_ASSERT(assetHandle.guid.isValid(), "");
            R_CORE_ASSERT(static_cast<bool>(std::is_base_of_v<AssetBase, T>), "");
            auto assetIt = assetCache.find(assetHandle.guid);
            if (assetIt == assetCache.end())
            {
                return nullptr;
            }
            auto ptr = std::dynamic_pointer_cast<T>(assetIt->second);
            R_CORE_ASSERT(ptr != nullptr, "");
            return ptr;
        }

        void RemoveAsset(const xg::Guid& id)
        {
            assetCache.erase(id);
        }

        friend class AssetLoader;
    };
}
