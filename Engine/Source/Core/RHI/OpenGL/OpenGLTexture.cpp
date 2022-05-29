#include "OpenGLTexture.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "Assert.hpp"
#include "TextureLoader.hpp"
#include <glad/glad.h>

using namespace RightEngine;

OpenGLTexture::OpenGLTexture(const std::string& path)
{
    TextureLoader loader;
    const auto [data, spec] = loader.Load(path);
    specification = spec;
    Generate(data.data());
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
        case TextureFormat::RGB32F:
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGB32F,
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
