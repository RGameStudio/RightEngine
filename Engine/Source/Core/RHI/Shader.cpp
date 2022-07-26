#include "Shader.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include "Vulkan/VulkanShader.hpp"
#include "OpenGL/OpenGLShader.hpp"

using namespace RightEngine;

std::shared_ptr<Shader> Shader::Create(const std::string& vertexShaderPath,
                                       const std::string& fragmentShaderPath)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::Vulkan:
            return std::make_shared<VulkanShader>(vertexShaderPath, fragmentShaderPath);
        case GPU_API::OpenGL:
        R_CORE_ASSERT(false, "");
        default:
        R_CORE_ASSERT(false, "Unknown GPU API!");

    }
}
