#pragma once

#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

namespace RightEngine
{
    class Shader;
    class Buffer;

    struct GraphicsPipelineDescriptor
    {
        glm::ivec2 extent;
        std::shared_ptr<Shader> shader;
        std::unordered_map<int, std::shared_ptr<Buffer>> vertexBuffers;
        std::unordered_map<int, std::shared_ptr<Buffer>> buffers;
    };

    struct RenderPassDescriptor
    {
        Format format;
    };
}