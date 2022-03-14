#pragma once

#include "Texture.hpp"
#include <memory>

namespace RightEngine
{
    class Material
    {
    public:
        const std::shared_ptr<Texture>& GetBaseTexture() const;
        void SetBaseTexture(const std::shared_ptr<Texture>& texture);

    private:
        std::shared_ptr<Texture> baseTexture;
    };
}
