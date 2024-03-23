#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

out vec2 UV;
out vec3 Color;
out float azimuth;
out float altitude;
out int merde;

uniform mat4 model; 
uniform mat4 viewMat;

layout(std140, binding = 0) uniform camAndProject
{
	mat4 view;
	mat4 projection;
	vec4 viewPosition;
};


//day phases uniforms :
uniform int currentPhase;
uniform float currentPhaseNextPhaseDist;
uniform float currentHourCurrentPhaseBaseHourDist;

vec3 rgb(float red, float blue, float green);
vec4 placeCloud(vec4 pos); //along the hemisphere
mat4 BuildTranslation(vec3 delta);  //from https://stackoverflow.com/questions/33807535/translation-in-glsl-shader
void calcColor(vec4 pos);


vec3 dayPhaseHorizonColor[4] = vec3[]
(
	/*dawn*/ rgb(141, 95, 146),
	/*daytime*/ rgb(215, 250, 255),
	/*twilight*/ rgb(145, 58, 39),
	/*night*/ rgb(18, 50, 73)
);

vec3 dayPhaseSkyGradient[8] = vec3[]
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

mat4 newModel;

void main()
{
	vec4 pos = vec4(inPos.xyz, 1.0);

	placeCloud(pos);
	pos = projection * viewMat * newModel * pos;

	gl_Position = pos.xyww; //set the z value to w(=1.0) value so it fails all depth test 

	UV = inUV;
}


vec4 placeCloud(vec4 pos)
{
	vec4 squareCenterSpherePos;
	vec4 newPos;


	 altitude = radians (10);
	 azimuth = radians(0);

	altitude += radians(10) *  gl_InstanceID ;
	azimuth += radians(30)  *  gl_InstanceID ;
	 
	float x = sin(altitude) * cos(azimuth);
	float y = sin(altitude) * sin(azimuth);
	float z = cos(altitude);

	/*
	x = 0;
	y = 1;
	z = 0 ;
	*/

	squareCenterSpherePos = vec4(x,y,z,1.0) ;
	merde = gl_VertexID;
	switch (gl_VertexID)
	{
		case 0 :
			newPos.x = squareCenterSpherePos.x + 0.5;
			newPos.y = squareCenterSpherePos.y + 0.5;
			newPos.z = z;

			newModel = BuildTranslation(newPos.xyz)*model;
			break;
		
		case 1:
			newPos.x = squareCenterSpherePos.x + 0.5;
			newPos.y = squareCenterSpherePos.y - 0.5;
			newPos.z = z;

			newModel = BuildTranslation(newPos.xyz)*model;
			break;

		case 2:
			newPos.x = squareCenterSpherePos.x - 0.5;
			newPos.y = squareCenterSpherePos.y - 0.5;
			newPos.z = z;

			newModel = BuildTranslation(newPos.xyz)*model;
			break;

		case 3:
			newPos.x = squareCenterSpherePos.x - 0.5;
			newPos.y = squareCenterSpherePos.y + 0.5;
			newPos.z = z;

			newModel = BuildTranslation(newPos.xyz)*model;
			break;
	}

	return newPos;
}

mat4 BuildTranslation(vec3 delta)
{
	return mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(delta, 1.0));
}

void calcColor(vec4 pos)
{
	float posAzimuth = atan(pos.z, pos.x);

	if (posAzimuth < 0)
	{
		posAzimuth = abs(posAzimuth);
	}


	float posAltitude = acos(pos.y);

	vec3 firstColorGradient;
	vec3 secondColorGradient;

	//horizon gradient : 
	if (posAltitude > radians(15.0)) //horizon line 
	{

		int nextPhase = currentPhase + 1;
		if (nextPhase >= 4)
			nextPhase = 0;


		firstColorGradient = dayPhaseHorizonColor[currentPhase] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseHorizonColor[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		nextPhase = currentPhase * 2 + 2;
		if (nextPhase >= 8)
			nextPhase = 0;

		secondColorGradient = dayPhaseSkyGradient[currentPhase * 2] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		Color = firstColorGradient + posAltitude * ((secondColorGradient - firstColorGradient) / radians(15.0));
	}

	else
	{
		int nextPhase = currentPhase * 2 + 2;
		if (nextPhase >= 8)
			nextPhase = 0;

		firstColorGradient = dayPhaseSkyGradient[currentPhase * 2] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		nextPhase = currentPhase * 2 + 3;
		if (nextPhase >= 8)
			nextPhase = 1;


		secondColorGradient = dayPhaseSkyGradient[currentPhase * 2 + 1] * (1 - (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist))) + dayPhaseSkyGradient[nextPhase] * (1 / (currentPhaseNextPhaseDist / currentHourCurrentPhaseBaseHourDist));

		Color = firstColorGradient + posAltitude * ((secondColorGradient - firstColorGradient) / radians(90.0 - 15.0));
	}

}

vec3 rgb(float red, float blue, float green)
{
	return vec3(red / 255.0, blue / 255.0, green / 255.0);
}
