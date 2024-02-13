#pragma stage vertex
#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    fragColor = aColor;
}

#pragma stage end

#pragma stage fragment
#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}

#pragma stage end