#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 f_UV;
out vec3 f_Normal;
out vec3 f_WorldPos;
out mat3 f_TBN;

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