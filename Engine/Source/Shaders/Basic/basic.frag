#version 330 core

// Vertex shader data
out vec4 color;
in vec2 fUV;
in vec3 fNormal;

// Textures
uniform sampler2D baseTexture;
uniform bool hasBaseTexture;

// Material
uniform vec4 baseColor;

// Lighting
uniform float ambientStrength;
uniform vec3 ambientColor;
uniform bool hasAmbient;

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
}
