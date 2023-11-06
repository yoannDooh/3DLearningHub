#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TextCoord;

uniform sampler2D catTexture;
uniform sampler2D grassTexture;



void main()
{
	FragColor = texture(catTexture, TextCoord)* vec4(ourColor, 1.0);
}
