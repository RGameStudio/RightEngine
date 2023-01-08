#include "AssetManager.hpp"
#include "MaterialLoader.hpp"

using namespace RightEngine;

AssetManager& AssetManager::Get()
{
    static AssetManager instance;
    return instance;
}

const AssetHandle& AssetManager::GetDefaultTexture() const
{
    static bool wasLoaded = false;

    if (!wasLoaded)
    {
        defaultTexture = GetLoader<TextureLoader>()->Load("/Assets/Textures/editor_default_image.png");
        wasLoaded = true;
    }

    return defaultTexture;
}

const AssetHandle& AssetManager::GetDefaultMaterial() const
{
    static bool wasLoaded = false;

    if (!wasLoaded)
    {
        defaultMaterial = GetLoader<MaterialLoader>()->Load();
        wasLoaded = true;
    }

    return defaultMaterial;
}
