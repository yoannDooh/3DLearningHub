#version 420 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 normalVec;



layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPos;
};

uniform mat4 model;
uniform float outLineWeight;

void main()
{
	gl_Position = projection * view * model * vec4(inPos.xyz+normalVec*outLineWeight, 1.0);
}
