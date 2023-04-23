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

layout(push_constant) uniform ConstantBuffer
{
	mat4 u_LightSpaceMatrix;
    mat4 dummy1;
};

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Transform * vec4(aPosition, 1.0);
}  