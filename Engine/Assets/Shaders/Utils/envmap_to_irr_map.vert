#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 0) out vec3 f_LocalPos;

layout(binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    f_LocalPos = aPos;
    gl_Position =  u_Projection * u_View * vec4(f_LocalPos, 1.0);
}
