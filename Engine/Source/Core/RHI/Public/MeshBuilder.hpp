#pragma once

#include "Components.hpp"
#include <memory>

namespace RightEngine
{
    class MeshBuilder
    {
    public:
        static std::shared_ptr<Mesh> CubeGeometry();
        static std::shared_ptr<Mesh> PlaneGeometry();
        static std::shared_ptr<Mesh> QuadGeometry();
    };
}
