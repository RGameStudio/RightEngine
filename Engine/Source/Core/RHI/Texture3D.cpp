#include "Texture3D.hpp"
#include "Assert.hpp"
#include "Renderer.hpp"
#include "OpenGL/OpenGLTexture3D.hpp"

using namespace RightEngine;

std::shared_ptr<Texture3D> Texture3D::Create(const std::array<std::string, 6>& texturesPath)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture3D>(texturesPath);
        default:
            R_CORE_ASSERT(false, "GPU API");
            return nullptr;
    }
}
