#version 450 core
layout (location = 0) out vec4 aAlbedo;

layout(binding = 3) uniform sampler2D u_FinalImage;

layout(binding = 12) uniform UBSceneRendererSettings
{
    float u_Gamma;
};

struct VertexOutput
{
    vec2 UV;
};

layout(location = 0) in VertexOutput Output;

void main() {
    vec3 color = texture(u_FinalImage, Output.UV).rgb;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/u_Gamma));

    aAlbedo = vec4(color, 1.0);
}
