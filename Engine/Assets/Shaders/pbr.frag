#version 450 core
layout (location = 0) out vec4 aAlbedo;

struct VertexOutput
{
    vec2 UV;
    vec3 Normal;
    vec3 WorldPos;
    mat3 TBN;
    vec4 CameraPosition;
};

layout(location = 0) in VertexOutput Output;

layout (binding = 2) uniform UBMaterialData
{
    vec4	u_AlbedoV;
    float	u_MetallicV;
    float	u_RoughnessV;
};

layout(binding = 3) uniform sampler2D	u_Albedo;
layout(binding = 4) uniform sampler2D	u_Normal;
layout(binding = 5) uniform sampler2D	u_Metallic;
layout(binding = 6) uniform sampler2D	u_Roughness;
layout(binding = 7) uniform sampler2D	u_AO;
layout(binding = 8) uniform samplerCube u_IrradianceMap;
layout(binding = 9) uniform samplerCube u_PrefilterMap;
layout(binding = 10) uniform sampler2D	u_BRDFLUT;
layout(binding = 13) uniform sampler2D	u_ShadowMap;

struct Light
{
    vec4	color;
    vec4	position;
    vec4	rotation;
    float	intensity;
    int		type;
    float	radiusInner;
    float	radiusOuter;
    mat4	lightSpace;
};

layout(binding = 11) uniform LightBuffer
{
    Light	u_Light[30];
    int		u_LightsAmount;
    vec3	dummy;
};

float CalculateDirectionalShadow(vec4 fragPosLightSpace, vec4 lightPos, vec3 fragPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Output.Normal);
    vec3 lightDir = normalize(vec3(lightPos) - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

const mat4 bias = mat4( 
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0,
  0.5, 0.5, 0.0, 1.0 );

#define PI 3.1415926535897932384626433832795

// From http://filmicgames.com/archives/75
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 prefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(u_PrefilterMap, R, lodf).rgb;
	vec3 b = textureLod(u_PrefilterMap, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness, Light light, vec3 albedo)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	vec3 lightColor = light.intensity * vec3(light.color);

	vec3 color = vec3(0.0);
	float r = roughness;
	if (roughness <= 0.05f)
	{
		r = 0.05f;	
	}

	if (dotNL > 0.0) 
	{
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, r); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, r);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_SchlickR(dotNV, F0, r);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);	
		spec = clamp(spec, vec3(0), vec3(1.0f));
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
		color += (kD * albedo / PI + spec) * lightColor * dotNL;
	}

	return color;
}

vec3 calculateNormal()
{
	if (texture(u_Normal, Output.UV).xyz == vec3(1))
	{
		return Output.Normal; 
	}
	vec3 tangentNormal = texture(u_Normal, Output.UV).xyz * 2.0 - 1.0;
	return normalize(Output.TBN * tangentNormal);
}

void main()
{		
	vec3 N = calculateNormal();

	vec3 V = normalize(vec3(Output.CameraPosition) - Output.WorldPos);
	vec3 R = reflect(-V, N); 

	vec3 albedo = pow(texture(u_Albedo, Output.UV).rgb, vec3(2.2)) * vec3(u_AlbedoV);
	float metallic = u_MetallicV;
	float roughness = u_RoughnessV;

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < u_LightsAmount; i++) {
		Light light = u_Light[i];
		vec3 L = normalize(vec3(light.position) - Output.WorldPos);
		vec4 fragPosLightSpace = light.lightSpace * vec4(Output.WorldPos, 1.0f);
		Lo += specularContribution(L, V, N, F0, metallic, roughness, light, albedo);
	}   
	
	vec2 brdf = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(u_IrradianceMap, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * albedo;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	  
	vec3 ambient = (kD * diffuse + specular) * texture(u_AO, Output.UV).rrr;
	
	vec3 color = ambient + Lo;

	// Tone mapping
	color = Uncharted2Tonemap(color * 1.0f);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / 2.2));

	aAlbedo = vec4(color, 1.0);
}