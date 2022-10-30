#include "AssetLoader.hpp"
#include "AssetManager.hpp"

using namespace RightEngine;

void AssetLoader::OnRegister(AssetManager* aManager)
{
    manager = aManager;
}

void AssetLoader::RemoveAsset(const xg::Guid& guid)
{
    manager->RemoveAsset(guid);
}
