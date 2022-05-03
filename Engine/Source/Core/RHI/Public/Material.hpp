#pragma once

#include "Texture.hpp"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace RightEngine
{
    struct MaterialData
    {
        glm::vec4 fallbackColor{ 1.0f, 0.0f, 1.0f, 1.0f };
        bool hasAlbedo;
        bool hasNormal;
        bool hasSpecular;
        bool hasMetallic;
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
