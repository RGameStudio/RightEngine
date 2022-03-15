#include "Material.hpp"

using namespace RightEngine;

const std::shared_ptr<Texture>& RightEngine::Material::GetBaseTexture() const
{
    return baseTexture;
}

void RightEngine::Material::SetBaseTexture(const std::shared_ptr<Texture>& texture)
{
    baseTexture = texture;
}

const MaterialData& Material::GetMaterialData() const
{
    return materialData;
}

void Material::SetMaterialData(const MaterialData& data)
{
    materialData = data;
}
