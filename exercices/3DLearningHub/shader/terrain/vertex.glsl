#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTextCoord;

out vec2 textCoord;

void main()
{
	gl_Position = vec4(inPos.xyz, 1.0);
	textCoord = inTextCoord;
}