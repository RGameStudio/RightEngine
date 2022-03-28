#pragma once

#include "Texture.hpp"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace RightEngine
{
    struct MaterialData
    {
        glm::vec3 ambient{ 1.0f, 0.5f, 0.31f };
        glm::vec3 diffuse{ 1.0f, 0.5f, 0.31f };
        glm::vec3 specular{ 0.5f, 0.5f, 0.5f};
        float shininess{ 32.0f };
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
