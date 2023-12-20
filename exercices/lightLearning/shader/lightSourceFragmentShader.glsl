#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	vec3 color = vec3(34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f);
	FragColor = vec4(color,1.0);
}
