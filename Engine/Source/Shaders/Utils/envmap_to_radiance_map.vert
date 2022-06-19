#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 f_LocalPos;

uniform mat4 u_ViewProjection;

void main()
{
    f_LocalPos = aPos;
    gl_Position =  u_ViewProjection * vec4(f_LocalPos, 1.0);
}