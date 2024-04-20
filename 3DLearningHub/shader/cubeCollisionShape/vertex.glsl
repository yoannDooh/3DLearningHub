#version 460 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
}