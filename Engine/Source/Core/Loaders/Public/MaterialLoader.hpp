#pragma once

#include "AssetBase.hpp"
#include "AssetLoader.hpp"

namespace RightEngine
{
    class MaterialLoader : public AssetLoader
    {
    public:
        AssetHandle Load();
        AssetHandle LoadWithGUID(const xg::Guid& guid);

    private:

    };
}