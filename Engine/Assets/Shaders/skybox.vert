#version 450 core
layout(location = 0) in vec3 aPosition;

layout(binding = 1) uniform UBCameraData
{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

layout(location = 0) out vec3 f_WorldPos;

void main()
{
    f_WorldPos = aPosition;
    vec4 pos = u_ViewProjection * vec4(aPosition.xyz, 1.0);
    gl_Position = pos.xyww;
}
