layout (location = 0) out vec4 aColorId;

struct VertexOutput
{
    vec2 UV;
    vec3 Normal;
    vec3 WorldPos;
    mat3 TBN;
    vec4 CameraPosition;
};

layout(location = 0) in VertexOutput Output;

layout (binding = 13) uniform UBColorId
{
    vec4 u_ColorId;
};

void main()
{
    aColorId = vec4(u_ColorId);
}