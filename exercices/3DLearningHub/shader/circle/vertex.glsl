#version 330 core
layout(location = 0) in vec2 centerPos;

/*
out VS_OUT{
	vec3 color;
} vs_out;
*/
 

void main()
{
	gl_Position = vec4(centerPos.x, centerPos.y, 0.0, 1.0);
}