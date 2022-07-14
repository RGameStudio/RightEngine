#pragma once

#include "Components.hpp"
#include <memory>

namespace RightEngine
{
    class MeshBuilder
    {
    public:
        static std::shared_ptr<MeshComponent> CubeGeometry();
        static std::shared_ptr<MeshComponent> PlaneGeometry();
        static std::shared_ptr<MeshComponent> QuadGeometry();
    };
}
