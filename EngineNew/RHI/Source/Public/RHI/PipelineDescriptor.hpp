#pragma once

#include <RHI/ShaderDescriptor.hpp>
#include <glm/vec4.hpp>
#include <EASTL/vector_map.h>
#include <cstdint>

namespace rhi
{

class Shader;
class RenderPass;

enum class CompareOp : uint8_t
{
    LESS = 0,
    LESS_OR_EQUAL,
    GREATER
};

enum class CullMode : uint8_t
{
    NONE = 0,
    BACK,
    FRONT
};

struct PipelineDescriptor
{
	std::shared_ptr<Shader>         m_shader;
    std::shared_ptr<RenderPass>     m_pass;
    CompareOp	                    m_depthCompareOp = CompareOp::LESS;
    CullMode	                    m_cullMode = CullMode::BACK;
    bool                            m_offscreen = true;
};

}