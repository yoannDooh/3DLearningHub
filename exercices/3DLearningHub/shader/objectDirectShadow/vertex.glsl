#version 460 core

layout(location = 0) in vec3 inPos;

uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
	gl_Position = lightSpaceMat * model * vec4(inPos.xyz, 1.0);
}