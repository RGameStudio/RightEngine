#pragma once

#include "Texture3D.hpp"
#include <string>

namespace RightEngine
{
    struct EnvironmentContext
    {
        std::shared_ptr<Texture3D> envMap;
    };

    class EnvironmentMapLoader
    {
    public:
        EnvironmentMapLoader() = default;
        ~EnvironmentMapLoader() = default;

        void Load(const std::string& path, bool flipVertically = false);

        inline const EnvironmentContext& GetEnvironmentContext() const { return currentContext; }
    private:
        EnvironmentContext currentContext;
    };
}
