#include "Shader.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include "OpenGL/OpenGLShader.hpp"

using namespace RightEngine;

std::shared_ptr<Shader> Shader::Create(const std::string& vertexShaderPath,
                                       const std::string& fragmentShaderPath)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLShader>(vertexShaderPath, fragmentShaderPath);
        default:
        R_CORE_ASSERT(false, "Unknown GPU API!");

    }
}
