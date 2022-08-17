#pragma once

#include "VertexBufferLayout.hpp"
#include <string>

namespace RightEngine
{
    enum class ShaderType
    {
        NONE = 0,
        VERTEX,
        FRAGMENT
    };
    typedef ShaderType ShaderStage;

    struct ShaderDescriptor
    {
        std::string name;
        std::string path;
        ShaderType type = ShaderType::NONE;
    };

    struct ShaderProgramDescriptor
    {
        std::vector<ShaderDescriptor> shaders;
        VertexBufferLayout layout;
    };
}