#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalVec;
layout(location = 2) in vec2 aTextCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;


void main()
{
	gl_Position = lightSpaceMat * model * vec4(aPos.xyz, 1.0);
}