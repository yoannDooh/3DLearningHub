#version 330 core
out vec4 FragColor;
uniform vec3 lightColor;

in vec2 texCoords;

void main()
{
	FragColor = vec4(lightColor.xyz,1.0);
}
