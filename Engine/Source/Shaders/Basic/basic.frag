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
uniform float ambient;
uniform float specular;
uniform int shininess;

// Lighting
uniform vec3 ambientColor;
uniform bool hasAmbient;

uniform int pointLightAmount;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];

// Misc data
uniform vec3 cameraPos;

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

    vec4 fragAmbientColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (hasAmbient)
    {
        fragAmbientColor = vec4(ambient * ambientColor, 1.0);
    }

    vec4 fragDiffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 fragSpecularColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (pointLightAmount > 0)
    {
        // TODO: Calculate multiple point lights
        vec3 normal = normalize(fNormal);
        vec3 lightDir = normalize(pointLightPos[0] - fPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * pointLightColor[0];
        fragDiffuseColor = vec4(diffuse, 1.0);

        vec3 viewDir = normalize(cameraPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        fragSpecularColor = vec4(specular * spec * pointLightColor[0], 1.0);
    }

    color = clamp((fragDiffuseColor + fragAmbientColor + fragSpecularColor),
                    vec4(0.0, 0.0, 0.0, 0.0),
                    vec4(1.0, 1.0, 1.0, 1.0)) * color;
//    color = (fragDiffuseColor + fragAmbientColor + fragSpecularColor) * color;
}
