﻿#version 460 core
layout(triangles, equal_spacing) in;
#define M_PI 3.1415926535897932384626433832795

//in vec2 vertexTextCoord[];

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;
out mat3 TBN;


layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};

uniform mat4 model;
uniform float radius;
uniform mat4 sphereTranslation;


mat4 newModel;
vec3 elipseCenterCoord = vec3(0.0f, 0.0f, 0.0f);

mat4 buildTranslation(vec3 delta); //from https://stackoverflow.com/questions/33807535/translation-in-glsl-shader
vec4 barycentricInterpolation(float u, float v,float w, vec4 data001, vec4 data010, vec4 data100);
vec4 extrude(vec4 pos);
vec3 slerpNormal(vec3 pos);
vec2 sleprUv(vec3 pos);
int[2] twoSmallestElemIndex(float[6] array);
void setSphereTranslation(vec3 pos);
void calcUv(vec3 pos,float u,float v,float w); //part du principe que le radius est de 1 et que le centre de la sphere est au coord 0,0,0

mat3 tbnMat(float u, float v, vec3 normal, vec3 pos, mat3 model);

void main()
{
	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	//TextCoord = bilinearInterpolation(u, v, vertexTextCoord[0], vertexTextCoord[1], vertexTextCoord[2], vertexTextCoord[3]);
	//vec3 pos = vec3(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

	vec4 pos  = barycentricInterpolation(u,v,w, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

	pos = extrude(pos);
	calcUv(pos.xyz,u,v,w);

	Normal = normalize(slerpNormal(pos.xyz));

	setSphereTranslation(pos.xyz);

	gl_Position = projection*view* newModel * vec4(pos);
	FragPos = vec3(newModel * vec4(pos.xyz, 1.0));

	//TBN = tbnMat(UV.x, UV.y, normalVec.xyz, pos.xyz, normalsModel);

	/*
	if (activateNormalMap == 1)
	{
		mat3 normalsModel = mat3(transpose(inverse(model)));
		TBN = tbnMat(u, v, area1, normalVec.xyz, pos.xyz, normalsModel);

	}
	*/
}

vec4 barycentricInterpolation(float u, float v, float w, vec4 data001, vec4 data010, vec4 data100)
{
	return ( u * data001 + v * data010 + w * data100 );
}

vec4 extrude(vec4 pos)
{
	float radius = 1.0;
	pos.xyz = normalize(pos.xyz);
	pos = radius * pos;

	return pos;
}

vec3 slerpNormal(vec3 pos)
{
	vec3 interpolatedNormal;

	vec3 normalsOfReference[6] = { vec3(1.0,0.0,0.0),vec3(0.0,0.0,-1.0),vec3(0.0,1.0,0.0),vec3(-1.0,0.0,0.0),vec3(0.0,0.0,1.0),vec3(0.0,-1.0,0.0) };  

	vec3 vertexOfReferencePos[6] = { vec3(radius,0.0,0.0),vec3(0.0,0.0,-radius),vec3(0.0,radius,0.0),vec3(-radius,0.0,0.0),vec3(0.0,0.0,radius),vec3(0.0,-radius,0.0) };  

	float angleBetweenSection = M_PI/2; 

	float posAndVertexDist[6]; //LE NOM DE VARIABLE EST TROP MOCHE 

	int pointOfControlIndex[2];

	for (int index = 0; index < 6; ++index)
	{
		posAndVertexDist[index] = length(pos - vertexOfReferencePos[index]);
	}

	pointOfControlIndex = twoSmallestElemIndex(posAndVertexDist);

	if (pointOfControlIndex[0] == 5 && pointOfControlIndex[1] == 0)
	{
		int temp = pointOfControlIndex[0];
		pointOfControlIndex[0] = pointOfControlIndex[1];
		pointOfControlIndex[1] = temp;
	}


	float tParameter = (acos( (dot(pos,vertexOfReferencePos[pointOfControlIndex[0]] ) ) / (radius*radius) )) / angleBetweenSection;

	interpolatedNormal = ((sin((1 - tParameter) * angleBetweenSection)) / sin(angleBetweenSection)) * normalsOfReference[pointOfControlIndex[0]];

	interpolatedNormal += ((sin(tParameter * angleBetweenSection)) / sin(angleBetweenSection)) * normalsOfReference[pointOfControlIndex[1]];

	return interpolatedNormal;
}

vec2 sleprUv(vec3 pos)
{
	vec2 interpolatedUv;

	vec2 uOfReference[6] = { vec2(0.5,0.5),vec2(0.25,0.5),vec2(0.0,1.0),vec2(1.0,0.0),vec2(0.75,0.5),vec2(0.5,0.0) };  

	vec3 vertexOfReferencePos[6] = { vec3(radius,0.0,0.0),vec3(0.0,0.0,-radius),vec3(0.0,radius,0.0),vec3(-radius,0.0,0.0),vec3(0.0,0.0,radius),vec3(0.0,-radius,0.0) };

	float angleBetweenSection = M_PI / 2;

	float posAndVertexDist[6];

	int pointOfControlIndex[2];

	for (int index = 0; index < 6; ++index)
	{
		posAndVertexDist[index] = length(pos - vertexOfReferencePos[index]);
	}

	pointOfControlIndex = twoSmallestElemIndex(posAndVertexDist);

	if (pointOfControlIndex[0] == 5 && pointOfControlIndex[1] == 0)
	{
		int temp = pointOfControlIndex[0];
		pointOfControlIndex[0] = pointOfControlIndex[1];
		pointOfControlIndex[1] = temp;
	}

	float tParameter = (acos((dot(pos, vertexOfReferencePos[pointOfControlIndex[0]])) / (radius * radius))) / angleBetweenSection;

	interpolatedUv = ((sin((1 - tParameter) * angleBetweenSection)) / sin(angleBetweenSection)) * uOfReference[pointOfControlIndex[0]];

	interpolatedUv += ((sin(tParameter * angleBetweenSection)) / sin(angleBetweenSection)) * uOfReference[pointOfControlIndex[1]];

	return interpolatedUv;
}

int[2] twoSmallestElemIndex(float[6] array)
{
	int result[2];
	int smallestElemIndex = 0;

	for (int index1 = 1; index1 < 6; ++index1)
	{
		if (array[index1] < array[smallestElemIndex])
			smallestElemIndex = index1;
	}
	result[0] = smallestElemIndex;

	smallestElemIndex = 0;
	for (int index1 = 1; index1 < 6; ++index1)
	{
		if(index1 == result[0])
			continue;

		if (array[index1] < array[smallestElemIndex])
			smallestElemIndex = index1;
	}
	result[1] = smallestElemIndex;

	return result;
}

mat4 buildTranslation(vec3 delta)
{
	return mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(delta, 1.0));
}

void setSphereTranslation(vec3 pos)
{
	vec4 sphereCenter = sphereTranslation * vec4(elipseCenterCoord.xyz, 1.0);

	newModel = model * buildTranslation(vec3(sphereCenter.xyz + pos));
}

//https://mft-dev.dk/uv-mapping-sphere/
void calcUv(vec3 pos, float u, float v, float w)
{
//	vec3 rightVertexPos = pos() //approximaiton

	UV.x = 0.5 + atan(pos.z, pos.x) / (2*M_PI);
	UV.y = 0.5 + asin(pos.y) / M_PI;

	//fix seam artefact
	int offset = 1 / 100;
	vec4 rightVertexPos = barycentricInterpolation(u+offset, v+offset, w+offset, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
	rightVertexPos = extrude(rightVertexPos);
	vec2 rightVertexUv = vec2( 0.5 + atan(rightVertexPos.z, rightVertexPos.x) / (2 * M_PI), 0.5 + asin(rightVertexPos.y) / M_PI);

	if (UV.x - rightVertexUv.x >= 0.5)
	{
		UV = 1 + UV;
	}
}


/*
mat3 tbnMat(float u, float v, Area area, vec3 normal, vec3 pos, mat3 model)
{

	vec3 tangent;
	vec3 bitangent;

	float xOffset = 0.1 / chunkWidth; //used to move sample across x axis
	float yOffset = 0.1 / chunkHeight; //used to move sample across y axis

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
	normal = normalize(model * normal);
	//bitangent = normalize(model * bitangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	bitangent = normalize(cross(normal, tangent));


	return mat3(tangent, normal, bitangent);
}
*/
