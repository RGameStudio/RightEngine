#version 330 core
layout (location = 0) out vec4 aAlbedo;

in vec3 f_TexCoords;

uniform samplerCube u_Skybox;

void main()
{
    aAlbedo = texture(u_Skybox, f_TexCoords);
}