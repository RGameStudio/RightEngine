#version 450 core

layout(location = 0) out vec4 aColor;

void main()
{             
	gl_FragDepth = gl_FragCoord.z;
}  