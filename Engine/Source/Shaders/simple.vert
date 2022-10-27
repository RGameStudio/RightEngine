#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform TransformConstant
{
    mat4 transform;
} transform;

layout(binding = 1) uniform SceneUBO
{
    mat4 view;
    mat4 projection;
} sceneUbo;

void main()
{
    vec4 pos = sceneUbo.view[3];
//    debugPrintfEXT("X: %f Y: %f Z: %f W: %f ", pos.x, pos.y, pos.z, pos.w);
    gl_Position = sceneUbo.projection * sceneUbo.view * transform.transform * vec4(aPosition, 1.0);
    fragColor = vec3(1.0);
    fragTexCoord = aUv;
}