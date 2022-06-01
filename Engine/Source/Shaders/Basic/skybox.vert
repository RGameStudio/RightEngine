#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 f_TexCoords;

void main()
{
    f_TexCoords = vec3(aPosition.x, aPosition.y, aPosition.z);
    vec4 pos = u_Projection * u_View * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
}
