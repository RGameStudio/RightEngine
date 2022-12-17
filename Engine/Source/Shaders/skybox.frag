#version 450 core
layout (location = 0) out vec4 aAlbedo;

layout(location = 0) in vec3 f_WorldPos;

layout(binding = 3) uniform samplerCube u_Skybox;

void main()
{
    vec3 envColor = textureLod(u_Skybox, f_WorldPos, 1.2).rgb;
    aAlbedo = vec4(envColor, 1.0);
}