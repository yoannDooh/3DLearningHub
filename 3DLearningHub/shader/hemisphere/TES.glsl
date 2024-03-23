﻿#version 460 core
layout(triangles, equal_spacing) in;
#define M_PI 3.141592653
#define M_PI_OVER_2 1.57079632679
#define M_PI_OVER_3 1.0471975512
#define M_PI_OVER_6 0.52359877559


out vec3 Normal;
out vec2 UV;
out mat3 TBN;
out vec3 Color;
out vec3 FragPos;


uniform mat4 model; //pour l'abaisser un peu
uniform mat4 projection;
uniform mat4 view;
uniform float elaspedTime; //timeInGame (in minutes)
uniform float timeAccelarator=1.0f;


//day phases uniforms :
uniform int currentPhase;
uniform float currentPhaseNextPhaseDist;
uniform float currentHourCurrentPhaseBaseHourDist;

vec4 barycentricInterpolation(float u, float v,float w, vec4 data001, vec4 data010, vec4 data100);
vec4 extrude(vec4 pos);
void calcNormal(vec4 pos);
void calcUv(vec3 pos); //part du principe que le radius est de 1 et que le centre de la sphere est au coord 0,0,0
void animateUv(vec4 pos);
void calcColor(vec4 pos);
vec3 rgb(float red, float blue, float green);


vec3 dayPhaseHorizonColor [4] = vec3[]
(
	/*dawn*/ rgb(141, 95, 146),
	/*daytime*/ rgb(215, 250, 255),
	/*twilight*/ rgb(145, 58, 39),
	/*night*/ rgb(18, 50, 73)
);

vec3 dayPhaseSkyGradient [8] = vec3[]
(			
			/*dawn*/ 
	/*c1*/  rgb(104, 115, 146),
	/*c2*/  rgb(42, 132, 203),

			/*daytime*/
	/*c1*/  rgb(123, 195, 237),
	/*c2*/	rgb(53, 134, 189),

			/*twilight*/
	/*c1*/	rgb(154, 72, 18),
	/*c2*/	rgb(25, 48, 71),

			/*night*/
	/*c1*/  rgb(5, 16, 21),
	/*c2*/	rgb(0, 0, 0)
);

void main()
{
	//coord of vertex within abstract patch in range [0,1]
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	vec4 pos  = barycentricInterpolation(u,v,w, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
	pos = extrude(pos);

	calcNormal(pos);
	calcUv(Normal);
	animateUv(pos);
	calcColor(pos);

	
	FragPos = pos.xyz;
	pos = projection*view*model*vec4(pos.xyz,1.0);
	gl_Position = pos.xyww; //set the z value to w(=1.0) value so it fails all depth test 

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

void calcNormal(vec4 pos)
{
	Normal = normalize(pos.xyz);
}

//https://mft-dev.dk/uv-mapping-sphere/
void calcUv(vec3 normal)
{

 	UV.x = 1 - ( 0.5 + asin(normal.x) / M_PI );
	UV.y = 1 - ( asin(normal.y) / M_PI_OVER_2 );

}

void animateUv(vec4 pos)
{
	float posAzimuth;
	float posAltitude = acos(pos.y);
	float newAzimuth;
	vec3 newUvPos;

	posAzimuth = atan(pos.z,pos.x);

	newAzimuth = posAzimuth + (M_PI/2400.0)*elaspedTime;
	
	newUvPos.y = pos.y;
	newUvPos.x = sin(posAltitude) * cos(newAzimuth)  ;
	newUvPos.z = sin(posAltitude) * sin(newAzimuth) ;

	calcUv(normalize(newUvPos));
}

void calcColor(vec4 pos)
{
	float posAzimuth = atan(pos.z,pos.x);

	if (posAzimuth < 0)
	{
		posAzimuth = abs(posAzimuth) ;
	}


	float posAltitude = acos(pos.y);

	vec3 firstColorGradient;
	vec3 secondColorGradient;

	//horizon gradient : 
	if (posAltitude > radians(15.0) ) //horizon line 
	{

		int nextPhase = currentPhase + 1;
		if (nextPhase >= 4)
			nextPhase = 0;


		firstColorGradient = dayPhaseHorizonColor[currentPhase] * (1 - (1/(currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist) ) ) + dayPhaseHorizonColor[nextPhase] * (1/(currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		nextPhase = currentPhase * 2 + 2;
		if (nextPhase >= 8)
			nextPhase = 0;

		secondColorGradient = dayPhaseSkyGradient[currentPhase*2] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		Color = firstColorGradient + posAltitude * ((secondColorGradient - firstColorGradient) / radians(15.0));
	}

	else
	{
		int nextPhase = currentPhase * 2 + 2;
		if (nextPhase >= 8)
			nextPhase = 0;

		firstColorGradient = dayPhaseSkyGradient[currentPhase*2] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		nextPhase = currentPhase * 2 + 3;
		if (nextPhase >= 8)
			nextPhase = 1;


		secondColorGradient = dayPhaseSkyGradient[currentPhase*2+1] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		Color = firstColorGradient + posAltitude * ((secondColorGradient - firstColorGradient) / radians(90.0-15.0) );
	}
	
}

vec3 rgb(float red, float blue, float green)
{
	return vec3(red / 255.0, blue / 255.0, green / 255.0);
}


