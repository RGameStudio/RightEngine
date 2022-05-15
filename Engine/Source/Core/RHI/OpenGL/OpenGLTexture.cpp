#include "OpenGLTexture.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "Assert.hpp"
#include <stb_image.h>
#include <glad/glad.h>

using namespace RightEngine;

OpenGLTexture::OpenGLTexture(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* localBuffer = stbi_load(Path::ConvertEnginePathToOSPath(path).c_str(),
                                           &specification.width,
                                           &specification.height,
                                           &specification.componentAmount,
                                           0);
    // TODO: Move that to a separate texture loader
    const std::string hdr = ".hdr";
    if (std::equal(hdr.rbegin(), hdr.rend(), path.rbegin()))
    {
        specification.format = TextureFormat::RGB16F;
    }

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
    if (localBuffer)
    {
        Generate(localBuffer);
        R_CORE_INFO("Load texture at path {0} successfully. {1}x{2} {3} components!", path,
                                                                        specification.width,
                                                                        specification.height,
                                                                        specification.componentAmount);
    }
    else
    {
        R_CORE_ERROR("Failed to load texture at path: {0}", path);
    }
    stbi_image_free(localBuffer);
}

OpenGLTexture::OpenGLTexture(const TextureSpecification& aSpecification, const void* data)
{
    specification = aSpecification;
    Generate(data);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &id);
}

void OpenGLTexture::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, id);
}

void OpenGLTexture::UnBind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::Generate(const void* buffer)
{
    R_CORE_ASSERT(buffer && specification.width > 0
                         && specification.height > 0
                         && specification.componentAmount > 0
                         && specification.format != TextureFormat::None, "Texture data is incorrect!");



    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    switch (specification.format)
    {
        case TextureFormat::RED8:
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RED,
                         specification.width,
                         specification.height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
            break;
        case TextureFormat::RGB8:
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGB,
                         specification.width,
                         specification.height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
            break;
        case TextureFormat::RGBA8:
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGBA,
                         specification.width,
                         specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            break;
        case TextureFormat::RGB16F:
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGB16F,
                         specification.width,
                         specification.height, 0, GL_RGB, GL_FLOAT, buffer);
            break;
        default:
            R_CORE_ASSERT(false, "Unsupported texture format!");
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    UnBind();
}
