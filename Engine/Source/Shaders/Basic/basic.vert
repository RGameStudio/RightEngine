#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fPos;
out vec2 fUV;
out vec3 fNormal;

void main()
{
    fUV = uv;
    fNormal = mat3(transpose(inverse(model))) * normal;
    fPos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4((position), 1.0);
}
