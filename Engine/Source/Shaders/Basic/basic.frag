#version 330 core

out vec4 color;
in vec2 texCoord;

uniform sampler2D baseTexture;
uniform bool hasBaseTexture;

uniform vec4 baseColor;

void main()
{
    if (hasBaseTexture)
    {
        vec4 texColor = texture(baseTexture, texCoord);
        color = texColor;
    }
    else
    {
        color = baseColor;
    }

}
