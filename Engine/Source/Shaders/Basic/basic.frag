#version 330 core

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;

in vec2 f_UV;
in vec3 f_Normal;

struct Material {
    vec4 fallbackColor;
    bool hasAlbedo;
};
uniform Material u_material;

uniform sampler2D u_albedoTexture;

void main()
{
    if (u_material.hasAlbedo)
    {
        albedo = texture(u_albedoTexture, f_UV);
    }
    else
    {
        albedo = u_material.fallbackColor;
    }

    normal = vec4(f_Normal, 1.0f);
}
