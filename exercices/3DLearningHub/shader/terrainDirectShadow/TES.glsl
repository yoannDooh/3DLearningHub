#version 460 core
layout(quads, fractional_even_spacing) in;

in vec2 vertexTextCoord[];

uniform sampler2D heightMap;

uniform mat4 model;
uniform mat4 lightSpaceMat;

vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01);
vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);
float heightCurve(float height); //should make it a bezier curve

void main()
{
	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec2 textCoord = bilinearInterpolation(u,v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec4 pos = bilinearInterpolation(u, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);

	float height = texture(heightMap, textCoord).r;

	pos.y = heightCurve(height);

	gl_Position = lightSpaceMat*model*vec4(pos.xyz, 1.0);

}					
																															//			3	2
																															//		  0,1   1,1
vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01)							//			+---+
{																															//	lefData	|   | rightData
	vec2 leftData = data00 + v * (data01 - data00);																			//			+---+																					
	vec2 rightData = data10 + v * (data11 - data10);																		//		   0,0   1,0
	return  vec2(leftData + u * (rightData - leftData));																	//			0    1

}

vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01)
{																															
	vec4 leftData = data00 + v * (data01 - data00);																																									//	^																						//	|																						//	v
	vec4 rightData = data10 + v * (data11 - data10);																		
	return  vec4(leftData + u * (rightData - leftData));
}

float heightCurve(float height)
{
	//return (height*50 - 10);
	return (height);
}