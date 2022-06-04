#version 450 core
layout (location = 0) out vec4 aAlbedo;
layout (location = 1) out vec4 aNormal;

in vec2 f_UV;
in vec3 f_Normal;
in vec3 f_WorldPos;

layout(std140, binding = 0) uniform MaterialData
{
    vec4 u_Albedo;
    float u_Metallic;
    float u_Roughness;
    bool u_HasAlbedo;
    bool u_HasNormal;
    bool u_HasMetallic;
    bool u_HasRoughness;
    bool u_HasAO;
};

uniform sampler2D u_Textures[5];
uniform samplerCube u_IrradianceMap;

// lights
vec3 lightPositions[4];
vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_Textures[1], f_UV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(f_WorldPos);
    vec3 Q2  = dFdy(f_WorldPos);
    vec2 st1 = dFdx(f_UV);
    vec2 st2 = dFdy(f_UV);

    vec3 N   = normalize(f_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    lightPositions[0] = vec3(0, 0, -15);
    lightPositions[1] = vec3(10, 0, -15);
    lightPositions[2] = vec3(14, 0, -15);
    lightPositions[3] = vec3(0, 10, -15);
    lightColors[0] = vec3(150, 150, 150);
    lightColors[1] = vec3(247, 239, 79);
    lightColors[2] = vec3(150, 150, 150);
    lightColors[3] = vec3(150, 150, 150);

    vec3 albedo;
    vec3 N;
    float metallic;
    float roughness;
    float ao;

    if (u_HasAlbedo)
    {
        albedo = pow(texture(u_Textures[0], f_UV).rgb, vec3(2.2));
    }
    else
    {
        albedo = u_Albedo.rgb;
    }

    if (u_HasNormal)
    {
        N = getNormalFromMap();
    }
    else
    {
        N = f_Normal;
    }

    if (u_HasMetallic)
    {
        metallic = texture(u_Textures[2], f_UV).r;
    }
    else
    {
        metallic = u_Metallic;
    }

    if (u_HasRoughness)
    {
        roughness = texture(u_Textures[3], f_UV).r;
    }
    else
    {
        roughness = u_Roughness;
    }

    if (u_HasAO)
    {
        ao = texture(u_Textures[4], f_UV).r;
    }
    else
    {
        ao = 1.0f;
    }

    vec3 V = normalize(camPos - f_WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - f_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - f_WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    aAlbedo = vec4(color, 1.0);
    aNormal = vec4(N, 1.0);
}