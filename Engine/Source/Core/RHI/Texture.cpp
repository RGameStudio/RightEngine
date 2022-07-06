#include "Texture.hpp"
#include "Renderer.hpp"
#include "Assert.hpp"
#include "OpenGL/OpenGLTexture.hpp"

using namespace RightEngine;

std::shared_ptr<Texture> Texture::Create(const TextureSpecification& aSpecification, const std::vector<uint8_t>& data)
{
    R_CORE_ASSERT(aSpecification.type != TextureType::NONE, "");
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
                                         const CubemapFaces& faces)
{
    R_CORE_ASSERT(aSpecification.type == TextureType::CUBEMAP, "");
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLTexture>(aSpecification, faces);
        default:
        R_CORE_ASSERT(false, "GPU API");
            return nullptr;
    }
}

