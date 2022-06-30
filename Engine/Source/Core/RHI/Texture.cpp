#include "Texture.hpp"
#include "Renderer.hpp"
#include "Assert.hpp"
#include "OpenGL/OpenGLTexture.hpp"

using namespace RightEngine;

std::shared_ptr<Texture> Texture::Create(const TextureSpecification& aSpecification, const std::vector<uint8_t>& data)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture>(aSpecification, data);
        default:
        R_CORE_ASSERT(false, "GPU API");
            return nullptr;
    }
}

std::shared_ptr<Texture> Texture::Create(const TextureSpecification& aSpecification,
                                         const std::array<std::vector<uint8_t>, 6>& data)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture>(aSpecification, data);
        default:
        R_CORE_ASSERT(false, "GPU API");
            return nullptr;
    }
}

