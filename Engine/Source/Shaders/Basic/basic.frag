#version 330 core

out vec4 color;
in vec2 texCoord;

uniform sampler2D baseTexture;
uniform bool hasBaseTexture;

void main()
{
    if (hasBaseTexture)
    {
        vec4 texColor = texture(baseTexture, texCoord);
        color = texColor;
    }
    else
    {
        color = vec4(1.0, 1.0, 1.0, 1.0);
    }

}
