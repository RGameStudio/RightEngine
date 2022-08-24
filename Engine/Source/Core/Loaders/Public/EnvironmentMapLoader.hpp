#pragma once

#include "Texture.hpp"
#include <string>

namespace RightEngine
{
    struct EnvironmentContext : public AssetBase
    {
        ASSET_BASE()

        std::shared_ptr<Texture> envMap;
        std::shared_ptr<Texture> irradianceMap;
        std::shared_ptr<Texture> prefilterMap;
        std::shared_ptr<Texture> brdfLut;

        //TODO: Add EDITOR compile flag
        std::shared_ptr<Texture> equirectangularTexture;
        std::string name;
    };

    struct EnvironmentMapLoaderContext
    {
        std::string path;
        bool flipVertically{ false };
        TextureDescriptor specification;
    };

    class EnvironmentMapLoader
    {
    public:
        EnvironmentMapLoader();
        ~EnvironmentMapLoader() = default;

        void Load(const std::string& path, bool flipVertically = false);

        inline const std::shared_ptr<EnvironmentContext>& GetEnvironmentContext() const
        { return environmentContext; }

    private:
        void ComputeEnvironmentMap();
        void ComputeIrradianceMap();
        void ComputeRadianceMap();
        void ComputeLUT();

    private:
        std::shared_ptr<EnvironmentContext> environmentContext;
        EnvironmentMapLoaderContext loaderContext;
    };
}
