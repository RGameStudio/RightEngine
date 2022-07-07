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

    mat3 normalMatrix = transpose(inverse(mat3(u_Transform)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    mat3 TBN = transpose(mat3(T, aBiTangent, N));
    f_TBN = TBN;

    gl_Position = u_ViewProjection * vec4(f_WorldPos, 1.0);
}