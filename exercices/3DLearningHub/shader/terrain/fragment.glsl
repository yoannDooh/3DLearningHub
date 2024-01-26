#version 410 core

out vec4 FragColor;

in vec2 textCoord;
in vec4 normalVec;
in float height;
in vec3 fragPos;


void main()
{
	FragColor = vec4(height, height, height, 1.0);
}