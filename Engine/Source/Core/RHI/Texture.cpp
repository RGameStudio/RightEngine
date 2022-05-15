#include "Texture.hpp"
#include "Renderer.hpp"
#include "Assert.hpp"
#include "OpenGL/OpenGLTexture.hpp"

using namespace RightEngine;

std::shared_ptr<Texture> Texture::Create(const TextureSpecification& aSpecification, const void* data)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::None:
            R_CORE_ASSERT(false, "");
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture>(aSpecification, data);
    }
}

std::shared_ptr<Texture> Texture::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::None:
            R_CORE_ASSERT(false, "");
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture>(path);
    }
}
