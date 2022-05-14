#include "Material.hpp"

using namespace RightEngine;

MaterialData& Material::GetMaterialData()
{
    return materialData;
}

TextureData& Material::GetTextureData()
{
    return textureData;
}
