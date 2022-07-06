#include "TextureLoader.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "Assert.hpp"
#include <stb_image.h>
#include <stb_image_write.h>

using namespace RightEngine;

namespace
{
    bool isHDR(const std::string& path)
    {
        const std::string hdr = ".hdr";
        bool isHdr = false;
        isHdr |= stbi_is_hdr(path.c_str());
        isHdr |= std::equal(hdr.rbegin(), hdr.rend(), path.rbegin());
        return isHdr;
    }
}

TextureLoader::TextureLoader(const TextureLoaderOptions& options) : options(options)
{}

std::pair<std::vector<uint8_t>, TextureSpecification> TextureLoader::Load(const std::string& path, bool flipVertically) const
{
    bool isHdr = isHDR(path);
    TextureSpecification specification;
    if (flipVertically)
    {
        stbi_set_flip_vertically_on_load(true);
    }
    else
    {
        stbi_set_flip_vertically_on_load(false);
    }

    void* buffer = nullptr;
    if (isHdr)
    {
        buffer = stbi_loadf(Path::ConvertEnginePathToOSPath(path).c_str(),
                                &specification.width,
                                &specification.height,
                                &specification.componentAmount,
                                0);
    }
    else
    {
        buffer = stbi_load(Path::ConvertEnginePathToOSPath(path).c_str(),
                                           &specification.width,
                                           &specification.height,
                                           &specification.componentAmount,
                                           0);
    }

    if (buffer)
    {
        R_CORE_INFO("Loaded texture at path {0} successfully. {1}x{2} {3} components!", path,
                    specification.width,
                    specification.height,
                    specification.componentAmount);
    }
    else
    {
        R_CORE_ERROR("Failed to load texture at path: {0}", path);
        R_CORE_ASSERT(false, "");
    }

    if (isHdr)
    {
        specification.format = TextureFormat::RGB32F;
    }

    if (specification.format == TextureFormat::NONE)
    {
        switch (specification.componentAmount)
        {
            case 1:
                specification.format = TextureFormat::RED8;
                break;
            case 3:
                specification.format = TextureFormat::RGB8;
                break;
            case 4:
                specification.format = TextureFormat::RGBA8;
                break;
            default:
                R_CORE_ASSERT(false, "");
        }
    }

    //TODO: Add checks for texture format and components amount

    std::vector<uint8_t> data;
    const size_t textureSize = specification.GetTextureSize();
    switch (specification.format)
    {
        case TextureFormat::RED8:
            R_CORE_ASSERT(specification.componentAmount == 1, "");
            break;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            R_CORE_ASSERT(specification.componentAmount == 3, "");
            break;
        case TextureFormat::RGBA8:
            R_CORE_ASSERT(specification.componentAmount == 4, "");
            break;
        default:
            R_CORE_ASSERT(false, "");
    }

    data.resize(textureSize);
    std::memcpy(data.data(), buffer, textureSize);

    stbi_image_free(buffer);
    return { data, specification };
}

std::shared_ptr<Texture> TextureLoader::CreateTexture(const std::string& path, bool flipVertically) const
{
    auto [data, spec] = Load(path, flipVertically);
    spec.type = TextureType::TEXTURE_2D;
    auto texture = Texture::Create(spec, data);
//    stbi_write_bmp("tex.bmp", spec.width, spec.height, spec.componentAmount, data.data());
    return texture;
}
