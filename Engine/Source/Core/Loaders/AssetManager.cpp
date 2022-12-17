#include "AssetManager.hpp"

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
