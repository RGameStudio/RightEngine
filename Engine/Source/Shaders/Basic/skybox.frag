#version 330 core
layout (location = 0) out vec4 aAlbedo;

in vec3 f_WorldPos;

uniform samplerCube u_Skybox;

void main()
{
    vec3 envColor = textureLod(u_Skybox, f_WorldPos, 1.2).rgb;

    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    aAlbedo = vec4(envColor, 1.0);
}