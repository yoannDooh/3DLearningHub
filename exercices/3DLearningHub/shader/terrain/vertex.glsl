#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTextCoord;

out vec2 textCoord;
uniform mat4 chunkModel;


void main()
{
	gl_Position = chunkModel*vec4(inPos.xyz, 1.0);
	textCoord = inTextCoord;
}