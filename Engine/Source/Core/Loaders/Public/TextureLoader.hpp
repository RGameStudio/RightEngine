#pragma once

#include "Texture.hpp"
#include <string>
#include <vector>

namespace RightEngine
{
    struct TextureLoaderOptions
    {
        Format format = Format::NONE;
        bool chooseFormat{ true };
        bool flipVertically{ false };
    };

    class TextureLoader
    {
    public:
        std::pair<std::vector<uint8_t>, TextureDescriptor> Load(const std::string& path,
                                  const TextureLoaderOptions& options = {}) const;

        std::shared_ptr<Texture> CreateTexture(const std::string& path,
                                               TextureType type = TextureType::TEXTURE_2D,
                                               const TextureLoaderOptions& options = {}) const;
    };
}
