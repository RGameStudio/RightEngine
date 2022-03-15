#pragma once

#include "Geometry.hpp"
#include <memory>

namespace RightEngine
{
    class GeometryBuilder
    {
    public:
        static std::shared_ptr<Geometry> CubeGeometry();
        static std::shared_ptr<Geometry> PlaneGeometry();
    };
}
