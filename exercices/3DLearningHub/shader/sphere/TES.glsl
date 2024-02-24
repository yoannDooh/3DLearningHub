﻿#version 460 core
layout(triangles, equal_spacing) in;
#define M_PI 3.1415926535897932384626433832795

//in vec2 vertexTextCoord[];

out vec3 fragPos;

layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};

uniform mat4 model;

vec4 barycentricInterpolation(float u, float v,float w, vec4 data001, vec4 data010, vec4 data100);
vec4 extrude(vec4 pos);

//vec4 barycentricInterpolation(float u, float v, vec4 data00, vec4 data10, vec4 data11, vec4 data01);

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

	gl_Position = projection*view*model * vec4(pos);
	fragPos = vec3(model * vec4(pos.xyz, 1.0));

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

/*
vec3 slerpNormal(vec3 pos)
{
	vec3 sphereTopNormal = vec3(0.0, 1.0, 0.0);
	vec3 sphereBottomNormal = vec3(0.0, -1.0, 0.0);
	
	float angleTopBottomVertex = 180.0; //in degree

	float angleWithPos = degrees ( acos((dot(pos, sphereTopNormal))  ) ); //no need to divide by individual length as it 1 because of the radius 

	float parameter = (angleWithPos - angleTopBottomVertex) / 360.0;
	
}
*/

