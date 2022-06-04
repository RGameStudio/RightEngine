#pragma once

#include "Texture.hpp"
#include <string>
#include <vector>

namespace RightEngine
{
    struct TextureLoaderOptions
    {
    };

    class TextureLoader
    {
    public:
        TextureLoader(const TextureLoaderOptions& options = TextureLoaderOptions());

        //TODO: Investigate why returning struct here leads to crash
        std::pair<std::vector<uint8_t>, TextureSpecification> Load(const std::string& path, bool flipVertically = false) const;
    private:
        TextureLoaderOptions options;
    };
}
