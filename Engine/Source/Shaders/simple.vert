#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform TransformConstant
{
    mat4 transform;
};

layout(binding = 0) uniform SceneUBO
{
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * transform * vec4(inPosition, 1.0);
    fragColor = inColor + vec3(1.0);
}