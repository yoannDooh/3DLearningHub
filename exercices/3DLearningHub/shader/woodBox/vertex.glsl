#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalVec; 
layout(location = 2) in vec2 aTextCoord;

out vec3 normal;
out vec3 FragPos;
out vec2 TextCoord;
out vec4 FragPosLightSpace;


layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};

uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
	gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	FragPosLightSpace = lightSpaceMat * vec4(FragPos,1.0);


	normal = mat3(transpose(inverse(model))) * normalVec;
	TextCoord = aTextCoord;
}