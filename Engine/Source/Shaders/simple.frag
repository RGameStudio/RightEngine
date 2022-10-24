#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 2) uniform usampler2D texSampler;

void main() {
    vec4 color = texture(texSampler, fragTexCoord);
//    const float gamma = 2.2;
//    color.rgb = pow(color.rgb, vec3(1.0/gamma));
    outColor = color;
}