#pragma once

#include <crossguid/guid.hpp>
#include <functional>

namespace RightEngine
{
    class AssetManager;

    class AssetLoader
    {
    public:
        virtual void OnRegister(AssetManager* aManager);
        virtual void RemoveAsset(const xg::Guid& guid);

    protected:
        AssetManager* manager;
    };
}