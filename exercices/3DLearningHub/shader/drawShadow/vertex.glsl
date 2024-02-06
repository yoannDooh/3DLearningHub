#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoords;

out vec3 fragPos;
out vec3 normal;
out vec2 textCoords;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;

void main()
{
	gl_Position = projection * view * model * vec4(inPos.xyz, 1.0);

	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * inNormal;
	textCoords = inTextCoords;
	fragPosLightSpace = lightSpaceMat * vec4(fragPos, 1.0);
}

