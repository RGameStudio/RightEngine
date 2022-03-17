#pragma once

#include "Texture.hpp"
#include <glm/vec4.hpp>
#include <memory>

namespace RightEngine
{
    struct MaterialData
    {
        glm::vec4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float specular{ 1.0f };
    };

    class Material
    {
    public:
        const std::shared_ptr<Texture>& GetBaseTexture() const;
        void SetBaseTexture(const std::shared_ptr<Texture>& texture);

        const MaterialData& GetMaterialData() const;
        void SetMaterialData(const MaterialData& data);

    private:
        std::shared_ptr<Texture> baseTexture;
        MaterialData materialData;
    };
}
