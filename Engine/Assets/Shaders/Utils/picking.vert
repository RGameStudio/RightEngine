#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(binding = 0) uniform UBTransformData
{
    mat4 u_Transform;
};

layout(binding = 1) uniform UBCameraData
{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

struct VertexOutput
{
    vec2 UV;
    vec3 Normal;
    vec3 WorldPos;
    mat3 TBN;
    vec4 CameraPosition;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    Output.UV = aUv;
    Output.Normal = mat3(u_Transform) * aNormal;
    Output.WorldPos = vec3(u_Transform * vec4(aPosition, 1.0));

    vec3 T = normalize(vec3(u_Transform * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(aBiTangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    Output.TBN = TBN;
    Output.CameraPosition = u_CameraPosition;

    gl_Position = u_ViewProjection * vec4(Output.WorldPos, 1.0);
}