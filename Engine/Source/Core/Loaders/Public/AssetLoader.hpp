#pragma once

#include "AssetBase.hpp"
#include <crossguid/guid.hpp>
#include <functional>

namespace RightEngine
{
    class AssetManager;

    class AssetLoader
    {
    public:
        virtual void OnRegister(AssetManager* aManager);
        virtual void RemoveAsset(const AssetHandle& handle);

    protected:
        AssetManager* manager{ nullptr };
    };
}