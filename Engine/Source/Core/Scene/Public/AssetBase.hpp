#pragma once

#include <string>

namespace RightEngine
{
    enum class AssetType
    {
        NONE = 0,
        IMAGE,
        MESH,
        ENVIRONMENT_MAP,
        SHADER
    };

    class AssetBase
    {
    public:
        virtual std::string GetClass() const = 0;

    public:
        std::string id;
        AssetType type;
    };
}

#define ASSET_BASE() virtual std::string GetClass() const override \
                              { return typeid(*this).name(); }