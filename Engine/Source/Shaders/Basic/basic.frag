#version 330 core

#define MAX_POINT_LIGHTS 10

// Vertex shader data
out vec4 color;
in vec3 fPos;
in vec2 fUV;
in vec3 fNormal;

// Textures
uniform sampler2D baseTexture;
uniform bool hasBaseTexture;

// Material
uniform vec4 baseColor;
uniform float specular;

// Lighting
uniform float ambientStrength;
uniform vec3 ambientColor;
uniform bool hasAmbient;

uniform int pointLightAmount;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];

void main()
{
    if (hasBaseTexture)
    {
        vec4 texColor = texture(baseTexture, fUV);
        color = texColor;
    }
    else
    {
        color = baseColor;
    }

    if (hasAmbient)
    {
        vec4 ambient = vec4(ambientStrength * ambientColor, 1.0);
        color = color * ambient;
    }

    if (pointLightAmount > 0)
    {
        // TODO: Calculate multiple point lights
        vec3 normal = normalize(fNormal);
        vec3 lightDir = normalize(pointLightPos[0] - fPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * pointLightColor[0];
        color = color + vec4(diffuse, 1.0) * color;
    }
}
