#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(binding = 0) uniform VP
{
    mat4 u_ViewProjection;
};

layout(binding = 1) uniform Transform
{
    mat4 u_Transform;
};

layout(location = 0) out vec2 f_UV;
layout(location = 1) out vec3 f_Normal;
layout(location = 2) out vec3 f_WorldPos;
layout(location = 3) out mat3 f_TBN;

void main()
{
    f_UV = aUv;
    f_Normal = mat3(u_Transform) * aNormal;
    f_WorldPos = vec3(u_Transform * vec4(aPosition, 1.0));

    vec3 T = normalize(vec3(u_Transform * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(aBiTangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    f_TBN = TBN;

    gl_Position = u_ViewProjection * vec4(f_WorldPos, 1.0);
}