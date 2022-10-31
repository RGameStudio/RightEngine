#version 450 core
layout(location = 0) in vec3 aPosition;

layout(binding = 0) uniform VP
{
    mat4 u_View;
    mat4 u_Projection;
};

layout(location = 0) out vec3 f_WorldPos;

void main()
{
    f_WorldPos = aPosition;
    vec4 pos = u_Projection * u_View * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
}
