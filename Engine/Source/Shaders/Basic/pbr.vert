#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 f_UV;
out vec3 f_Normal;
out vec3 f_WorldPos;

void main()
{
    f_UV = aUv;
    f_Normal = aNormal;
    f_WorldPos = vec3(u_Transform * vec4(aPosition, 1.0));
    gl_Position = u_ViewProjection * u_Transform * vec4((aPosition), 1.0);
}