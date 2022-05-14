#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 f_UV;
out vec3 f_Normal;
out vec3 f_WorldPos;

void main()
{
    f_UV = uv;
    f_Normal = normal;
    f_WorldPos = vec3(u_Transform * vec4(position, 1.0));
    gl_Position = u_ViewProjection * u_Transform * vec4((position), 1.0);
}