#version 330 core

out vec4 color;

in vec2 f_UV;

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
        color = texture(u_albedoTexture, f_UV);
    }
    else
    {
        color = u_material.fallbackColor;
    }
}
