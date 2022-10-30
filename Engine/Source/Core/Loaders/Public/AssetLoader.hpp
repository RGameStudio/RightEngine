#pragma once

#include "AssetBase.hpp"
#include <oneapi/tbb/task_group.h>
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
        virtual void WaitAllLoaders();

    protected:
        AssetManager* manager{ nullptr };
        mutable oneapi::tbb::task_group taskGroup;
    };
}