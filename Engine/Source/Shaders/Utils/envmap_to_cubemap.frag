#version 420 core
layout (location = 0) out vec4 aAlbedo;

in vec3 f_LocalPos;

layout(binding = 0) uniform sampler2D u_EquirectangularMap;

const vec2 kInvAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= kInvAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(f_LocalPos)); // make sure to normalize localPos
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

    aAlbedo = vec4(color, 1.0);
}
