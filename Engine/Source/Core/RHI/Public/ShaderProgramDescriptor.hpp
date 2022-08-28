#pragma once

#include "VertexBufferLayout.hpp"
#include "Utils.hpp"
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

    struct BufferRef
    {
        int slot;
        ShaderStage stage;

        bool operator==(const BufferRef& other) const
        {
            return slot == other.slot && stage == other.stage;
        }
    };

    struct BufferRefHash
    {
        size_t operator()(const BufferRef& p) const
        {
            size_t h = 0x1231;
            Utils::CombineHash(h, p.slot);
            Utils::CombineHash(h, p.stage);

            return h;
        }
    };

    struct ShaderReflection
    {
        std::unordered_map<BufferRef, BufferType, BufferRefHash> buffers;
        std::vector<int> textures;
    };

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
        ShaderReflection reflection;
    };
}