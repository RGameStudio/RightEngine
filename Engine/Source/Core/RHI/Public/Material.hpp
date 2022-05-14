#pragma once

#include "Texture.hpp"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace RightEngine
{
    struct MaterialData
    {
        glm::vec4 albedo{ 1.0f, 1.0f, 1.0f, 1.0f };
        float metallic{ 0.0f };
        float roughness{ 0.0f };
        int hasAlbedo{ false };
        int hasNormal{ false };
        int hasMetallic{ false };
        int hasRoughness{ false };
        int hasAO{ false };
    };

    struct TextureData
    {
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> normal;
        std::shared_ptr<Texture> metallic;
        std::shared_ptr<Texture> roughness;
        std::shared_ptr<Texture> ao;
    };

    struct Material
    {
        TextureData textureData;
        MaterialData materialData;
    };
}
