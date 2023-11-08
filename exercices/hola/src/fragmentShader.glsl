#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TextCoord;

uniform sampler2D catTexture;
uniform sampler2D grassTexture;
uniform float mixValue;



void main()
{
	FragColor = mix(texture(catTexture, vec2(-TextCoord.x,TextCoord.y) ),texture(grassTexture, TextCoord),0.2)* vec4(ourColor, 1.0);
	
	
	
	
	
}
