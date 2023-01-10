#include "MaterialLoader.hpp"
#include "Material.hpp"
#include "AssetManager.hpp"

using namespace RightEngine;

AssetHandle MaterialLoader::Load()
{
    const auto material = std::make_shared<Material>();
    return manager->CacheAsset(material, "", AssetType::MATERIAL);
}
