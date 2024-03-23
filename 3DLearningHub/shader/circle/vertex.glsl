#version 330 core
layout(location = 0) in vec3 centerPos;

/*
out VS_OUT{
	vec3 color;
} vs_out;
*/
 

void main()
{
	gl_Position = vec4(centerPos.xyz, 1.0);
}