#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 f_UV;

void main()
{
    f_UV = uv;
    gl_Position = u_ViewProjection * u_Transform * vec4((position), 1.0);
}
