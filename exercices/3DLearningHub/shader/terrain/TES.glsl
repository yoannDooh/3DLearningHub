#version 460 core
layout(quads, fractional_even_spacing) in;

in vec2 vertexTextCoord[];

struct Area {

	int chunkId;
	float[2] xRange; //first is xPosLEft and second xPosRight in WORLD unit, to indicate where the texture expand and until where it stretch 
	float[2] zRange; //same with z 
	float[2] yRange; //range in height of the texture

	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_displacement1;
	sampler2D texture_emission1;
	float shininess;
};

uniform Area area1;
uniform float inverseWidth; //je sais pas pourquoi la dljkgwsngfmkldjqasznbfjmdkoslgndfjomzfghjdsoqkljgsfkmdljkmflqshjgsdfl de ses muertos l'uniform elle est pas passé je vais peter mon crane 
uniform float inverseHeight;//même chose 
uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMat;



out float height;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec2 TextCoord;
out vec4 normalVec;


vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01);
vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);
vec4 vertexNormal(vec2 textCoord,vec3 pos);
//mat3 tbnMat(Chunk chunk,vec3 normal);

void main()
{

	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	TextCoord = bilinearInterpolation(u,v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec4 pos = bilinearInterpolation(u, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);

	height = texture(heightMap, TextCoord).r;

	pos.y = height*25-5; //go intégrer courbe de bézier ici 
	normalVec = vec4(0.0,1.0,0.0,0.0);

	normalVec = vertexNormal(TextCoord, pos.xyz); 

	gl_Position = projection*view*model*vec4(pos.xyz, 1.0);
	fragPos = vec3(model * vec4(pos.xyz,1.0) );
	fragPosLightSpace = lightSpaceMat * vec4(fragPos,1.0);																	//	^
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

vec4 vertexNormal(vec2 textCoord,vec3 pos)
{

	 float xOffset = 1.0 / 400.0; //le truc qu'on va se servir pour se deplacer sur x
	 float yOffset = 1.0 / 400.0; //le truc qu'on va se servir pour se deplacer 

	vec2 rightSampleCoord = vec2(textCoord.x + xOffset, textCoord.y); //coord of a sample of the texture a the right of current vertex
	vec2 topSampleCoord = vec2(textCoord.x, textCoord.y + yOffset); //coord of a sample of the texture a the top of current vertex

	vec3 rightVertex = vec3(pos.x+ xOffset*400.0, pos.y, pos.z); //the rightSample in world space
	vec3 topVertex = vec3(pos.x, pos.y, pos.z+ yOffset*400.0); //the topSample in world space

	if (rightSampleCoord.x <= 1 && rightSampleCoord.y <= 1 && topSampleCoord.x <= 1 && topSampleCoord.y <= 1) //do the sampled vertices texCoord are in the range of the texture
	{
		rightVertex.y = texture(heightMap, rightSampleCoord).r * 25-5;
		topVertex.y = texture(heightMap, topSampleCoord).r * 25-5;

		vec3 rightVector = rightVertex - pos;
		vec3 topVector = topVertex - pos;

		return normalize(vec4(cross(topVector.xyz, rightVector.xyz), 0.0));
	}

	else
		return vec4(0.0, 1.0 ,0.0, 0.0);
}


/*
mat3 tbnMat(Chunk chunk,vec3 normal)
{

	vec3 tangent;
	vec3 bitangent;

	// positions of the 4 edges of the whole chunk JE SAIS PAS ENCORE POUR LES NOMS genre c'est pas le chunk mais le terrain entier en tout cas le bout de terrain qui correspond à la height 
	vec3 pos1 = vec3(chunk.xRange[1],  0.0, chunk.yRange[0]);
	vec3 pos2 = vec3(chunk.xRange[1],  0.0, chunk.yRange[1]);
	vec3 pos3 = vec3(chunk.xRange[0], 0.0, chunk.yRange[1]);

	// texture coordinates
	vec2 uv10 = vec2(1.0, 0.0);
	vec2 uv11 = vec2(1.0, 1.0);
	vec2 uv01 = vec2(0.0, 1.0);

	vec3 edge1 = pos3 - pos2;
	vec3 edge2 = pos1 - pos3;

	vec2 deltaUV1 = uv11 - uv10;
	vec2 deltaUV2 = uv10 - uv11;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	
}
*/