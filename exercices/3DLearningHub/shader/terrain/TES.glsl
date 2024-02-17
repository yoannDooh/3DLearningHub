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
	float specularIntensity;
};

layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};

uniform sampler2D heightMap;
uniform int chunkId;
uniform int chunkWidth; //je sais pas pourquoi la dljkgwsngfmkldjqasznbfjmdkoslgndfjomzfghjdsoqkljgsfkmdljkmflqshjgsdfl de ses muertos l'uniform elle est pas passé je vais peter mon crane 
uniform int chunkHeight;//même chose 
uniform Area area1;
uniform int activateNormalMap = 1;

uniform mat4 model;
uniform mat4 lightSpaceMat;

out float Height;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec2 TextCoord;
out vec4 normalVec;
out mat3 TBN;


vec2 bilinearInterpolation(float u, float v, vec2 data00, vec2 data10, vec2 data11, vec2 data01);
vec4 bilinearInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);
vec4 vertexNormal(float u, float v,vec2 textCoord, vec3 pos);
mat3 tbnMat(float u, float v, Area area,vec3 normal, vec3 pos, vec2 textCoord, mat3 model);
float heightCurve(float height); //should make it a bezier curve

void main()
{


	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	TextCoord = bilinearInterpolation(u,v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec4 pos = bilinearInterpolation(u, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);

	Height = texture(heightMap, TextCoord).r;

	pos.y = heightCurve(Height);

	normalVec = vertexNormal(u,v,TextCoord, pos.xyz); 

	gl_Position = projection*view*model*vec4(pos.xyz, 1.0);
	fragPos = vec3(model * vec4(pos.xyz,1.0) );
	fragPosLightSpace = lightSpaceMat * vec4(fragPos,1.0);		


	if (activateNormalMap == 1)
	{
		mat3 normalsModel = mat3(transpose(inverse(model)));
		TBN = tbnMat(u, v, area1, normalVec.xyz, pos.xyz, TextCoord, normalsModel);

	}
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
	vec4 leftData = data00 + v * (data01 - data00);																																							//		   00   10																						//	^																						//	|																						//	v
	vec4 rightData = data10 + v * (data11 - data10);																		
	return  vec4(leftData + u * (rightData - leftData));
}

vec4 vertexNormal(float u, float v, vec2 textCoord, vec3 pos)
{

	float xOffset = 1.0 / chunkWidth; //used to move sample across x axis
	float yOffset = 1.0 / chunkHeight; //used to move sample across y axis

	vec2 rightSampleCoord = bilinearInterpolation(u + xOffset, v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);
	vec2 topSampleCoord = bilinearInterpolation(u, v + yOffset, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);

	vec3 rightVertex = vec3( bilinearInterpolation(u + xOffset, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position) );
	vec3 topVertex = vec3(bilinearInterpolation(u, v + yOffset, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position) );

	if (rightSampleCoord.x <= 1 && rightSampleCoord.y <= 1 && topSampleCoord.x <= 1 && topSampleCoord.y <= 1) //do the sampled vertices texCoord are in the range of the patch
	{
		rightVertex.y = texture(heightMap, rightSampleCoord).r; //* 25-5;
		topVertex.y = texture(heightMap, topSampleCoord).r; //* 25-5;

		vec3 rightVector = rightVertex - pos;
		vec3 topVector = topVertex - pos;

		return normalize(vec4(cross(topVector.xyz, rightVector.xyz), 0.0));
	}

	else
		return vec4(0.0, 1.0, 0.0, 0.0);
}

mat3 tbnMat(float u, float v, Area area, vec3 normal, vec3 pos, vec2 textCoord, mat3 model)
{
	vec3 tangent;
	vec3 bitangent;

	float xOffset = 1.0 / chunkWidth; //used to move sample across x axis
	float yOffset = 1.0 / chunkHeight; //used to move sample across y axis

	//positions
	vec3 pos0 = pos;
	vec3 pos1 = vec3(bilinearInterpolation(u + xOffset, v, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position));
	vec3 pos3 = vec3(bilinearInterpolation(u, v + yOffset, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position));

	// texture coordinates

	//transform area Coord from patch uv range to it's own uv [0,1] range 
	vec2 uv00; //uv of current vertex
	uv00.x = (pos0.x - area.xRange[0]) / (area.xRange[1] - area.xRange[0]);
	uv00.y = (pos0.z - area.zRange[0]) / (area.zRange[1] - area.zRange[0]);

	vec2 uv10;
	uv10.x = (pos1.x - area.xRange[0]) / (area.xRange[1] - area.xRange[0]);
	uv10.y = (pos1.z - area.zRange[0]) / (area.zRange[1] - area.zRange[0]);

	vec2 uv01;
	uv01.x = (pos3.x - area.xRange[0]) / (area.xRange[1] - area.xRange[0]);
	uv01.y = (pos3.z - area.zRange[0]) / (area.zRange[1] - area.zRange[0]);

	vec3 edge1 = pos0 - pos3;
	vec3 edge2 = pos1 - pos3;

	vec2 deltaUV1 = uv00 - uv01;
	vec2 deltaUV2 = uv10 - uv01;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	tangent = normalize(model * tangent);
	normal = normalize(model * tangent);
	bitangent = normalize(model * bitangent);

	return mat3(tangent, normal, bitangent);
}

float heightCurve(float height)
{
	return (height*50 - 10);
}