#include "MaterialLoader.hpp"
#include "Material.hpp"
#include "AssetManager.hpp"

using namespace RightEngine;

AssetHandle MaterialLoader::Load()
{
    const auto material = std::make_shared<Material>();
    R_CORE_ASSERT(manager, "")
    return manager->CacheAsset(material, "", AssetType::MATERIAL);
}

AssetHandle MaterialLoader::LoadWithGUID(const xg::Guid& guid)
{
    const auto material = std::make_shared<Material>();
    R_CORE_ASSERT(manager, "")
    return manager->CacheAsset(material, "", AssetType::MATERIAL, guid);
}
