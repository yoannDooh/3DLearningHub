#version 460 core
layout(quads, fractional_even_spacing) in;

in vec2 vertexTextCoord[];


uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;

out float height;
out vec2 TextCoord;
out vec4 normalVec;
out vec3 fragPos;
out vec4 fragPosLightSpace;


vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01);
vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);
vec4 vertexNormal(vec4 pos00, vec4 pos10, vec4 pos01);

void main()
{

	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	TextCoord = bilinearInterpolation(u,v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec4 pos = bilinearInterpolation(u, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);

	height = texture(heightMap, TextCoord).r;

	pos += vec4(0.0, 1.0, 0.0, 0.0) * height * 25 - 5;
	//pos += height - 3;

	vec4 pos00 = gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * height * 25 - 5; //jsuis pas trop sur de tout ça hein 
	vec4 pos10 = gl_in[1].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * height * 25 - 5; //jsuis pas trop sur de tout ça hein 
	vec4 pos01 = gl_in[3].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * height * 25 - 5; //jsuis pas trop sur de tout ça hein 

	normalVec = vertexNormal(pos00, pos10, pos01); //bah ça change r jsuis perdu là 

	gl_Position = projection*view*model*vec4(pos.xyz, 1.0);
	fragPos = vec3(model * vec4(pos.xyz,1.0) );
	fragPosLightSpace = lightSpaceMat * vec4(fragPos,1.0);																													//	^
}																															//	|
																															//	v		3	2
																															//	u->	   0,1   1,1
vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01)							//			+---+
{																															//	lefData	|   | rightData
	vec2 leftData = data00 + v * (data01 - data00);																			//			+---+																						//		   00   10																						//	^																						//	|																						//	v
	vec2 rightData = data10 + v * (data11 - data10);																		//		   0,0   1,0
	return  vec2(leftData + u * (rightData - leftData));																	//			0    1

}

vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01)
{																															
	vec4 leftData = data00 + v * (data01 - data00);																																							//		   00   10																						//	^																						//	|																						//	v
	vec4 rightData = data10 + v * (data11 - data10);																		
	return  vec4(leftData + u * (rightData - leftData));
}

vec4 vertexNormal(vec4 pos00, vec4 pos10, vec4 pos01)
{
	vec4 uVec = pos10 - pos00;
	vec4 vVec = pos01 - pos00;
	return normalize( vec4 (cross(-uVec.xyz, vVec.xyz),0)  ); //don't know why but normals are reversed that's why i added minus 

}