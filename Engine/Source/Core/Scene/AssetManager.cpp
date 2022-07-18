#include "AssetManager.hpp"

using namespace RightEngine;

AssetManager& AssetManager::Get()
{
    static AssetManager instance;
    return instance;
}
