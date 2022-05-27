#version 330 core
layout (location = 0) out vec4 aAlbedo;
layout (location = 1) out vec4 aNormal;
in vec3 f_LocalPos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(f_LocalPos)); // make sure to normalize localPos
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

    aAlbedo = vec4(color, 1.0);
}