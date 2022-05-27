#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

out vec3 f_LocalPos;

uniform mat4 u_ViewProjection;

void main()
{
    f_LocalPos = aPosition;
    gl_Position =  u_ViewProjection * vec4(f_LocalPos, 1.0);
}