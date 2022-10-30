#include "AssetLoader.hpp"
#include "AssetManager.hpp"

using namespace RightEngine;

void AssetLoader::OnRegister(AssetManager* aManager)
{
    manager = aManager;
}

void AssetLoader::RemoveAsset(const AssetHandle& handle)
{
    manager->RemoveAsset(handle);
}
