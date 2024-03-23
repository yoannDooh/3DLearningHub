#version 330 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform int effectIndex=-1;

const float offset_x = 1.0f / 800;
const float offset_y = 1.0f / 600;

vec2 offsets[9] = vec2[]
(
				//left							//center				//right
/*top*/			vec2(-offset_x, offset_y),		vec2(0.0f, offset_y),	vec2(offset_x, offset_y),
/*center*/		vec2(-offset_x, 0.0f),			vec2(0.0f, 0.0f),		vec2(offset_x, 0.0f),
/*bottom*/		vec2(-offset_x, -offset_y),		vec2(0.0f, -offset_y),	vec2(offset_x, -offset_y)
);

void main()
{
	vec3 color = texture(screenTexture, texCoords).rgb;

	if (effectIndex <= 2) //it's not a "kernel effect"
	{
		switch (effectIndex)
		{
		case 0: //inverse
			color = 1 - color;
			break;

		case 1: //greyScale
			float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
			color = vec3(average, average, average);
			break;
		}
	}

	else //"kernelEffect"
	{ 
		float kernel[9];
		vec3 sampleTex[9];

		switch (effectIndex)
		{
		case 2: //blur
			kernel = float[]
				(
				 	1/16,  2/16,  1/16,
					2/16,  4/16,  2/16,
					1/16,  2/16,  1/16
				);		  
			break;

		case 3: //EdgeDection 
			kernel = float[]
				(
				 	1,  1,  1,
					1,  -8,  1,
					1,  1,  1
				);		  
			break;
		}

		for (int index = 0; index < 9; ++index)
			sampleTex[index] = vec3(texture(screenTexture, texCoords.st + offsets[index] ) ) ;
		
		color = vec3(0.0);

		for (int index = 0; index < 9; ++index)
			color += sampleTex[index] * kernel[index];

	}


	FragColor = vec4(color, 1.0);
}
