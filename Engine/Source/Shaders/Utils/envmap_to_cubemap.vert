#version 450 core
layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 f_LocalPos;

layout(binding = 0) uniform ViewProjectionUniform
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    f_LocalPos = inPosition;
    gl_Position =  u_Projection * u_View * vec4(f_LocalPos, 1.0);
}