#include "Texture.hpp"
#include "Logger.hpp"
#include <stb_image.h>
#include <glad/glad.h>


RightEngine::Texture::Texture(const std::string& path)
{
    //stbi_set_flip_vertically_on_load(1);
    unsigned char* localBuffer = stbi_load(path.c_str(), &width, &height, &componentAmount, 0);
    if (localBuffer) {
        GLenum format = GL_RGBA;
        if (componentAmount == 1)
            format = GL_RED;
        else if (componentAmount == 3)
            format = GL_RGB;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, localBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        UnBind();
        R_CORE_INFO("Load texture at path {0} successfully!", path);
    }
    else {
        R_CORE_ERROR("Failed to load texture at path: {0}");
    }
    stbi_image_free(localBuffer);
}

void RightEngine::Texture::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void RightEngine::Texture::UnBind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RightEngine::Texture::~Texture()
{
    glDeleteTextures(1, &id);
}
