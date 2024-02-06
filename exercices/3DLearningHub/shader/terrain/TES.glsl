#version 460 core
layout(quads, fractional_even_spacing) in;

in vec2 vertexTextCoord[];



uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;
uniform float inverseWidth; //faudra changer le nom en mode plutot chunkWidth mais vazi pas encore décider sur les unités d'aires/volume
uniform float inverseHeight;

out float height;
out vec2 TextCoord;
out vec4 normalVec;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out int patchId; //debug Info


vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01);
vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);
vec4 vertexNormal(float u, float v,vec3 pos);

void main()
{

	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;


	TextCoord = bilinearInterpolation(u,v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec4 pos = bilinearInterpolation(u, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);

	height = texture(heightMap, TextCoord).r;

	pos.y = height * 25 - 5;

	normalVec = vertexNormal(u, v, pos.xyz); 

	gl_Position = projection*view*model*vec4(pos.xyz, 1.0);
	fragPos = vec3(model * vec4(pos.xyz,1.0) );
	fragPosLightSpace = lightSpaceMat * vec4(fragPos,1.0);																	//	^
	patchId = gl_PrimitiveID;
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

vec4 vertexNormal(float u, float v, vec3 pos)
{

	float xOffset = 1 / 400; //le truc qu'on va se servir pour se deplacer sur x
	float yOffset = 1 / 400; //le truc qu'on va se servir pour se deplacer 

	vec2 rightSampleCoord = vec2(u + xOffset, v); //coord of a sample of the texture a the right of current vertex
	vec2 topSampleCoord = vec2(u, v + yOffset); //coord of a sample of the texture a the top of current vertex

	vec3 rightVertex = vec3(pos.x+xOffset, 0.0, pos.z); //the rightSample in world space
	vec3 topVertex = vec3(pos.x, 0.0, pos.z+yOffset); //the topSample in world space

	rightVertex.y = texture(heightMap, rightSampleCoord).r * 25 - 5;
	topVertex.y = texture(heightMap, topSampleCoord).r * 25 - 5;


	vec3 rightVector = rightVertex - pos;
	vec3 topVector = topVertex - pos;
	
	return normalize( vec4 (cross(topVector.xyz, rightVector.xyz),0)  ); 
}