#include "Texture.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "Assert.hpp"
#include <stb_image.h>
#include <glad/glad.h>


RightEngine::Texture::Texture(const std::string& path)
{
    //stbi_set_flip_vertically_on_load(1);
    unsigned char* localBuffer = stbi_load(Path::ConvertEnginePathToOSPath(path).c_str(),
                                           &specification.width,
                                           &specification.height,
                                           &specification.componentAmount,
                                           0);
    if (localBuffer)
    {
        Generate(localBuffer);
        R_CORE_INFO("Load texture at path {0} successfully!", path);
    }
    else
    {
        R_CORE_ERROR("Failed to load texture at path: {0}", path);
    }
    stbi_image_free(localBuffer);
}

RightEngine::Texture::Texture(const TextureSpecification& specification, const void* data) : specification(specification)
{
    Generate(data);
}

void RightEngine::Texture::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, id);
}

void RightEngine::Texture::UnBind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RightEngine::Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void RightEngine::Texture::Generate(const void* data)
{
    R_CORE_ASSERT(data && specification.width > 0 && specification.height > 0, "Texture data is null!");
    GLenum format = GL_RGBA;
    if (specification.componentAmount == 1)
    {
        format = GL_RED;
    }
    else if (specification.componentAmount == 3)
    {
        format = GL_RGB;
    }
    else if (specification.componentAmount != 4)
    {
        R_CORE_ASSERT(false, "Unsupported components amount");
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, specification.width, specification.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    UnBind();
}
