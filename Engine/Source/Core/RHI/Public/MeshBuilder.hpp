#pragma once

#include "Components.hpp"
#include <memory>

namespace RightEngine
{
    class MeshBuilder
    {
    public:
        static Mesh* CubeGeometry();
        static Mesh* PlaneGeometry();
    };
}
