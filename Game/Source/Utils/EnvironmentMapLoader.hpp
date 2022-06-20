#pragma once

#include "Texture3D.hpp"
#include <string>

namespace RightEngine
{
    struct EnvironmentContext
    {
        std::shared_ptr<Texture3D> envMap;
        std::shared_ptr<Texture3D> irradianceMap;
        std::shared_ptr<Texture3D> prefilterMap;
        std::shared_ptr<Texture> brdfLut;
    };

    struct EnvironmentMapLoaderContext
    {
        std::string path;
        bool flipVertically{ false };
        TextureSpecification specification;
    };

    class EnvironmentMapLoader
    {
    public:
        EnvironmentMapLoader();
        ~EnvironmentMapLoader() = default;

        void Load(const std::string& path, bool flipVertically = false);

        inline const EnvironmentContext& GetEnvironmentContext() const
        { return environmentContext; }

    private:
        void ComputeEnvironmentMap();
        void ComputeIrradianceMap();
        void ComputeRadianceMap();
        void ComputeLUT();

    private:
        EnvironmentContext environmentContext;
        EnvironmentMapLoaderContext loaderContext;
    };
}
