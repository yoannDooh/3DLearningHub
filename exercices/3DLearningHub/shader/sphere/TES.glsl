﻿#version 460 core
layout(triangles, equal_spacing) in;
#define M_PI 3.1415926535897932384626433832795

//in vec2 vertexTextCoord[];

out vec3 FragPos;
out vec3 Normal;

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
int[2] twoSmallestElemIndex(float[6] array);
void setSphereTranslation(vec3 pos);


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

	Normal = normalize(slerpNormal(pos.xyz));

	setSphereTranslation(pos.xyz);

	gl_Position = projection*view* newModel * vec4(pos);
	FragPos = vec3(newModel * vec4(pos.xyz, 1.0));

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

	vec3 normalsOfReference[6] = { vec3(1.0,0.0,0.0),vec3(0.0,0.0,-1.0),vec3(0.0,1.0,0.0),vec3(-1.0,0.0,0.0),vec3(0.0,0.0,1.0),vec3(0.0,-1.0,0.0) }; //TODO: change the variable name 

	vec3 vertexOfReferencePos[6] = { vec3(radius,0.0,0.0),vec3(0.0,0.0,-radius),vec3(0.0,radius,0.0),vec3(-radius,0.0,0.0),vec3(0.0,0.0,radius),vec3(0.0,-radius,0.0) }; //TODO: change the variable name 

	float angleBetweenSection = 60.0; //in degree //TODO: change the variable name 

	float posAndVertexDist[6]; //LE NOM DE VARIABLE EST TROP MOCHE 
	for (int index = 0; index < 6; ++index)
	{
		posAndVertexDist[index] = length(pos - vertexOfReferencePos[index]);
	}

	int pointOfControlIndex[2] = twoSmallestElemIndex(posAndVertexDist);

	if (pointOfControlIndex[0] == 5 && pointOfControlIndex[1] == 0)
	{
		int temp = pointOfControlIndex[0];
		pointOfControlIndex[0] = pointOfControlIndex[1];
		pointOfControlIndex[1] = temp;
	}


	float tParameter = (acos((dot(pos,vertexOfReferencePos[pointOfControlIndex[0]])) / (radius*radius) )) / angleBetweenSection;

	interpolatedNormal = ((sin((1 - tParameter) * angleBetweenSection)) / sin(angleBetweenSection)) * vertexOfReferencePos[pointOfControlIndex[0]]; 

	interpolatedNormal += ((sin(tParameter * angleBetweenSection)) / sin(angleBetweenSection)) * vertexOfReferencePos[pointOfControlIndex[1]];

	return interpolatedNormal;
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


