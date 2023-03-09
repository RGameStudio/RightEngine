#version 450 core

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;

layout(std140, binding = 0) uniform Material
{
    vec4 u_FallbackColor;
    bool u_HasAlbedo;
    bool u_HasNormal;
    bool u_HasSpecular;
    bool u_HasMetallic;
};

in vec2 f_UV;
in vec3 f_Normal;

uniform sampler2D u_Textures[32];

void main()
{
    if (u_HasAlbedo)
    {
        albedo = texture(u_Textures[0], f_UV);
    }
    else
    {
        albedo = u_FallbackColor;
    }

    normal = vec4(f_Normal, 1.0f);
}
