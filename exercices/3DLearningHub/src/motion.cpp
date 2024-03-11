#define _USE_MATH_DEFINES

#include "../header/motion.h"
#include <iostream>
#include <cmath>

namespace Mouse
{
	float sensitivity = 0.15f;
	float lastX{ 400 };
	float lastY{ 300 };
	bool firstMouseInput{ true };
	float fov{ 45 };
}

namespace Time
{
	float deltaTime{}; // Time between current frame and last frame in sec
	float previousDelta{}; //Time of previous frame in sec
	float deltaSum{}; //summ of the seconds passed since the beggening (by adding all deltaTime)
	float currentFrameTime{};
	float lastFrameTime{};
	int sec{}; //how many entire seconds has passed
	int currentFrame{ 1 };
	int totalFrame{ 1 };
	float fps{};
	Time timeInGame{ 13,0 };
	DayPhases dayPhase{ daytime };

	std::map<DayPhases, std::array<Time, 2> > dayPhasesTime
	{
		 { dawn,{ Time::Time(6, 0), Time::Time(8, 30) } },
		 { daytime,{ Time::Time(8, 30), Time::Time(17, 30) } },
		 { sunset,{ Time::Time(17, 30), Time::Time(19, 30) } },
		 { night,{ Time::Time(19, 30), Time::Time(6, 0) } },
	};

	int totalMinInGame{};
	float timeAccelerator{1.0f};
}

namespace World
{
	float CameraSpeed{ 40.5 };
	Camera camera{ glm::vec3(-26.2968f, 46.3522f, -40.89f),  glm::vec3(26.2968f, -46.3522f, 40.89f), glm::vec3(0.0f, 1.0f, 0.0f), CameraSpeed };
	glm::mat4 view { glm::lookAt(camera.pos, camera.pos + camera.front, camera.up) };
	glm::mat4 projection {  glm::perspective(glm::radians(Mouse::fov), projectionWidth / projectionHeight,projectionNear, projectionFar)  };

	float projectionWidth { 800.0f };
	float projectionHeight{ 600.0f };
	float projectionNear{ 0.1f };
	float projectionFar{ 1000.0f };


	std::vector<Object> objects{};
	std::vector<Light::lightPoint> lightPoints{};
	std::vector<Light::SpotLight> spotLights{};
	std::array<Light::DirectLight, DIRECT_LIGHTS_NB> directLights{
		{
		rgb(214, 132, 49), //color
	//	{ -0.545f, 0.629f, -0.552f },//direction
		{ 0.0, -1.0f, 0.0 },
		{ 0.8f, 0.8f, 0.8f,	 },	//ambient
		{ 0.8f, 0.8f, 0.8f,	 },	//diffuse
		{ 1.0f,	 1.0f,  1.0f }, //specular 
		}
	};

	 Object sunObj;
	 float sunAzimuth{90};
	 float sunAltitude{35};

	 std::map<DayPhases, glm::vec3>sunLightColor{
		 {dawn,rgb(254, 197, 185)},
		 {daytime,rgb(252, 252, 253)},
		 {sunset,rgb(111, 1, 0) },
		 {night,rgb(9, 8, 28)}
	 };

	int mapWidth{};
	int mapHeight{};
}

namespace usrParameters
{
	std::map<EffectOption, bool> effectOption {};
	std::map<InfoOption, bool> infoOptions {};
}

//OBJECT CLASS
Object::Object(glm::vec3 pos)
{
	model = glm::translate(localOrigin, pos);
	pos = pos;
}

bool Object::isLightPointIdValid(int id)
{
	if (id == -1 || World::lightPoints.size() <= id)
		return false;

	else
		return true;
}

bool Object::isSpotLightIdValid(int id)
{
	if (id == -1 || World::lightPoints.size() >= id)
		return false;

	else
		return true;
}

void Object::move(glm::vec3 vector)
{
	model = model * glm::translate(localOrigin, vector);
	pos = glm::translate(localOrigin, vector) * glm::vec4(pos, 1.0f);

	//should add a parameter to decide how the light position is influenced by the object it's associated with position 
	//by default the light has the same position as the object 
	//update lightPoint pos
	if (!isLightPointIdValid(worldLighPointId))
		return;

	else
		World::lightPoints[worldLighPointId].pos = pos;

	//update spotlight pos
	if (!isSpotLightIdValid(worldSpotLightId))
		return;

	else
		World::spotLights[worldSpotLightId].pos = pos;
}

void Object::rotate(float degree, glm::vec3 rotateAxis)
{
	float rad{ glm::radians(degree) };

	model = model * glm::rotate(model, rad, rotateAxis);
	pos = glm::rotate(model, rad, rotateAxis) * glm::vec4(pos, 1.0f);

	if (!isLightPointIdValid(worldLighPointId))
		return;

	else
		World::lightPoints[worldLighPointId].pos = pos;

	//update spotlight pos
	if (!isSpotLightIdValid(worldSpotLightId))
		return;

	else
		World::spotLights[worldSpotLightId].pos = pos;
}

void Object::scale(glm::vec3 scaleVec)
{
	model = model * glm::scale(localOrigin, scaleVec);
	pos = glm::scale(localOrigin, scaleVec) * glm::vec4(pos, 1.0f);

	if (!isLightPointIdValid(worldLighPointId))
		return;

	else
		World::lightPoints[worldLighPointId].pos = pos;

	//update spotlight pos
	if (!isSpotLightIdValid(worldSpotLightId))
		return;

	else
		World::spotLights[worldSpotLightId].pos = pos;
}

void Object::rotatePlane(float degree)
{
	degree = glm::radians(degree) ;

	glm::mat4 rot{
		glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
				  0.0f, cos(degree), sin(degree), 0.0f,
				  0.0f, -sin(degree), cos(degree), 0.0f,
				  0.0f, 0.0f, 0.0f, 1.0) };

	localOrigin = rot * localOrigin;
}

void Object::updateLightPoint(glm::vec3 newValue, int memberIndex)
{
	try
	{
		if( !isLightPointIdValid(worldLighPointId) )
			throw(-1);

		switch (memberIndex)
		{
		case 0:
			World::lightPoints[worldLighPointId].color = newValue;
			break;

		case 1:
			World::lightPoints[worldLighPointId].pos = newValue;
			break;

		case 2:
			World::lightPoints[worldLighPointId].ambient = newValue;
			break;

		case 3:
			World::lightPoints[worldLighPointId].diffuse = newValue;
			break;

		case 4:
			World::lightPoints[worldLighPointId].specular = newValue;
			break;

		}
	}

	catch (int exeption)
	{
		std::cerr << "DEREFENCING NULL POINTER WITH animateLightsCube FUNCTION CALL" << std::endl;
	}

}

void Object::set(Shader& shader, glm::vec3 translationVec, glm::vec3 rotationAxis, float rotationDegree, glm::vec3 scaleVec)
{
	if (enableScale)
		scale(scaleVec);

	if (enableRotation)
		rotate(rotationDegree, rotationAxis);

	if (enableTranslation)
		move(translationVec);

	shader.use();
	shader.setMat4("model", model);
	shader.setFloat("material.shininess", materialShininess);

	if (isOrbiting)
	{
		shader.setFloat("cubeEdge", distFromCenter);
	}
}

void Object::setLightPoint(glm::vec3 color, glm::vec3 ambiant, glm::vec3 diffuse, glm::vec3 specular, float constant, float linearCoef, float squareCoef)
{
	Light::lightPoint lightPoint{};

	worldLighPointId = World::lightPoints.size();
	World::lightPoints.push_back(lightPoint);

	try
	{
		if (!isLightPointIdValid(worldLighPointId))
			throw(-1);

		World::lightPoints[worldLighPointId].color = color;


		World::lightPoints[worldLighPointId].ambient = ambiant;
		World::lightPoints[worldLighPointId].diffuse = diffuse;
		World::lightPoints[worldLighPointId].specular = specular;

		World::lightPoints[worldLighPointId].constant = constant;
		World::lightPoints[worldLighPointId].linearCoef = linearCoef;
		World::lightPoints[worldLighPointId].squareCoef = squareCoef;
	}

	catch (int exeption)
	{
		std::cerr << "DEREFENCING NULL POINTER TO LIGHT OBJECT" << std::endl;

	}
}

void Object::animate(Mesh mesh, Shader& shader, glm::vec3 translationVec, glm::vec3 rotationAxis, float rotationDegree, glm::vec3 scaleVec)
{
	if (enableTranslation)
		move(translationVec);

	if (enableRotation)
		rotate(rotationDegree, rotationAxis);


	if (enableScale)
		scale(scaleVec);

	shader.use();
	shader.setMat4("model", model);

	if (isGlowing)
	{
		shader.setFloat("emmissionStrength", glow() );
		shader.set3Float("emmissionColor", glowColor);
	}

	if (isOrbiting)
	{
		glm::mat4 orbitMat{};
		orbitMat = orbit();

		//update light pos
		pos = glm::vec3(orbitMat * glm::vec4(basePos, 1.0f));

		updateLightPoint(pos, 1);

		shader.set3Float("lightColor", World::lightPoints[worldLighPointId].color);
		shader.setMat4("model", model);
		shader.setMat4("orbit", orbitMat);
	}

	if (isOutLined)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		//animateObject(mesh,shader,translationVec,rotationDegree,rotationAxis,scaleVec);

		mesh.draw(shader);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthFunc(GL_ALWAYS);


		shaderOutline.use();
		glm::vec3 outlineColor{ rgb(43, 117, 190) };
		shaderOutline.set3Float("outLineColor", outlineColor);

		float weight = 0.009f;
		//float weight = 0.006f; is the best but need to find a way to extend the lines
		shaderOutline.setFloat("outLineWeight", weight);

		shaderOutline.setMat4("model", model);
		mesh.draw(shaderOutline);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glDepthFunc(GL_LESS);

		return;
	}

	mesh.draw(shader);
}

float Object::glow()
{
	float currentFrameGlow{ };

	if (Time::deltaSum != 0)
	{
		currentFrameGlow = glowStrenghtMax * sin((M_2_PI / glowDuration) * Time::deltaSum);
	}

	if (currentFrameGlow < 0)
		currentFrameGlow *= -1;

	return currentFrameGlow;
}

glm::mat4 Object::orbit()
{
	glm::mat4 orbit{};
	float xAxisValue{};
	float zAxisValue{};

	if (Time::deltaTime != 0)
	{
		xAxisValue = (orbitHorizontalAxis * cos(((2 * M_PI) / orbitDuration) * Time::deltaSum));
		zAxisValue = (orbitVerticalAxis * sin(((2 * M_PI) / orbitDuration) * Time::deltaSum));
	}

	orbit = glm::translate(localOrigin, glm::vec3(xAxisValue, 0.0f, zAxisValue));

	//object.move(glm::vec3(xAxisValue, 0.0f, zAxisValue) );

	return  orbit;
}

void Object::addToWorldObjects()
{
	worldObjId = World::objects.size();
	World::objects.push_back(*this);

}

void setupShadowMap(ShadowBuffer depthMap)
{
	//setup texture size and frameBuffer
	glViewport(0, 0, depthMap.SHADOW_WIDTH, depthMap.SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMap.id);
	glClear(GL_DEPTH_BUFFER_BIT);
}

//frameBuffer CLASS
FrameBuffer::FrameBuffer(bool activateBufferTex, bool activateRenderBuff)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (activateBufferTex)
		genFrameBuffTex(SCR_WIDTH,SCR_HEIGHT);

	if (activateRenderBuff)
		genRenderBuff();

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		std::cerr << "renderBuffer failed, error : " << error << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::genFrameBuffTex(int width, int height)
{
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
}

void FrameBuffer::genRenderBuff()
{
	glGenRenderbuffers(1, &renderId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderId);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderId);

}

//ShadowBuffer CLASS
void ShadowBuffer::genDepthMapBuff()
{
	SHADOW_WIDTH = 1024;
	SHADOW_HEIGHT = 1024;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	

	genDepthMapTex(SHADOW_WIDTH, SHADOW_HEIGHT);
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		std::cerr << "ShadowBuffer failed, error : " << error << std::endl;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowBuffer::genCubeMapBuff()
{
	SHADOW_WIDTH = 1024;
	SHADOW_HEIGHT = 1024;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	genCubeMapTex(SHADOW_WIDTH, SHADOW_HEIGHT);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		std::cerr << "renderBuffer failed, error : " << error;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowBuffer::genDepthMapTex(int width, int height)
{

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//elments outside of the fbo size won't be in shadow
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
}

void ShadowBuffer::genCubeMapTex(int width, int height)
{
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	for (unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT,GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texId, 0);

}

void ShadowBuffer::genDepthMapLightSpaceMat(float lightRange, glm::vec3 lightPos, glm::vec3 lookAtLocation)
{
	float near_plane{ 1.0f }, far_plane{ lightRange };
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(lookAtLocation), glm::vec3(0.0f, 1.0f, 0.0f));

	depthMapLightSpaceMat = lightProjection * lightView;
}

void ShadowBuffer::genCubeMapLightSpaceMat(float lightRange, glm::vec3 lightPos)
{
	float aspect = static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);

	float near_plane = 1.0f;
	float farPlane = lightRange;

	glm::mat4 projection = glm::perspective(glm::radians(90.0f),aspect, near_plane, farPlane);

	//set view for 6 faces 
	cubeMapLightSpaceMat[0] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	cubeMapLightSpaceMat[1] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	cubeMapLightSpaceMat[2] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	cubeMapLightSpaceMat[3] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	cubeMapLightSpaceMat[4] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	cubeMapLightSpaceMat[5] = projection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void setLighting()
{
	//directional light
	fillUbo2(0, -1);
	

	for (int lightIndex{}; lightIndex < World::lightPoints.size(); ++lightIndex)
	{
		fillUbo1(lightIndex,-1);
	}

	//do the same for spotLights
}

void updateTimeInGame()
{
	Time::timeInGame.sec += 60 * Time::timeAccelerator;

	//handle time values
	while (Time::timeInGame.sec >= 60)
	{
		++Time::timeInGame.min;
		++Time::totalMinInGame;

		if (Time::timeInGame.sec < 61)
			Time::timeInGame.sec = 0;

		if (Time::timeInGame.sec > 61)
		{
			int currentSec{ abs (60 - static_cast<int>(Time::timeInGame.sec) ) };
			Time::timeInGame.sec = currentSec;
		}
	}

	while (Time::timeInGame.min >= 60)
	{
		++Time::timeInGame.hour;
		Time::timeInGame.min=0;
	}

	while (Time::timeInGame.hour >= 24)
	{
		++Time::timeInGame.day;
		Time::timeInGame.hour = 0;
	}
	

	Time::dayPhase = night;

	if (Time::timeInGame.hour >= 6 && Time::timeInGame.hour <= 8)
	{
		Time::dayPhase = dawn;

		if (Time::timeInGame.hour == 8 && Time::timeInGame.min > 30)
			Time::dayPhase = daytime;

	}

	if (Time::timeInGame.hour > 8 && Time::timeInGame.hour <= 17 )
	{
		Time::dayPhase = daytime;

		if (Time::timeInGame.hour == 17 && Time::timeInGame.min > 30)
			Time::dayPhase = sunset;
	}

	if (Time::timeInGame.hour > 17 && Time::timeInGame.hour <= 19)
	{
		Time::dayPhase = sunset;

		if (Time::timeInGame.hour == 19 && Time::timeInGame.min > 30)
			Time::dayPhase = night;
	}
}

glm::mat4 calSunPos() 
{
	glm::mat4 sphereTranslation{};
	float xAxisValue{};
	float zAxisValue{};
	float yAxisValue{};
	//the radius of the spere is one 

	static int previousMin{};
	int minTimeSpan{};
	float secSpan{ Time::deltaTime*216000*Time::timeAccelerator }; //1sec in real life is 60h inGame so 216 000sec inGame 

	if (previousMin < Time::totalMinInGame)
	{
		minTimeSpan = Time::totalMinInGame - previousMin;
		previousMin += Time::totalMinInGame - previousMin;
	}

	World::sunAzimuth += (0.25) * minTimeSpan;

	if (World::sunAzimuth >= 360)
	{
		float degreeSpan{ World::sunAzimuth - 360.0f };
		World::sunAzimuth = 0;
		World::sunAzimuth += degreeSpan;
	}

	if (Time::timeInGame.hour > 1 && Time::timeInGame.hour <= 13)
	{
		World::sunAltitude += (41.0f / 360.0f) * static_cast<float>(minTimeSpan);
	}

	if (Time::timeInGame.hour > 13)
	{
		World::sunAltitude += -(41.0f / 360.0f) * static_cast<float>(minTimeSpan);
	}

	//xAxisValue = 100*sin(World::sunAltitude) * cos(World::sunAzimuth);
	//zAxisValue = 100*sin(World::sunAltitude) * sin(World::sunAzimuth);
	//yAxisValue = 10*cos(World::sunAltitude); 

	xAxisValue = 100 * sin( ( ((M_PI)/2) / 300) * Time::deltaSum) * cos( ( (2 * M_PI) / 300) * Time::deltaSum);
	zAxisValue = 100 * sin((((M_PI) / 2) / 300) * Time::deltaSum) * sin(((2 * M_PI) / 300) * Time::deltaSum);
	yAxisValue = 10 * cos((((M_PI) / 2) / 300) * Time::deltaSum);


	sphereTranslation = glm::translate(World::sunObj.localOrigin,glm::vec3(xAxisValue, yAxisValue, zAxisValue));

	return sphereTranslation;
}

//view & projection function
void updateViewProject()
{
	World::view = glm::lookAt(World::camera.pos, World::camera.pos + World::camera.front, World::camera.up);
	World::projection = glm::perspective(glm::radians(Mouse::fov), World::projectionWidth / World::projectionHeight, World::projectionNear, World::projectionFar);

	fillUbo0(0);
	fillUbo0(1);
}

void setEffect(Shader& shader, Effects effect)
{
	shader.use();
	shader.setInt("effectIndex", effect);
}

//diverse useful function
glm::vec3 rgb(float red, float blue, float green)
{
	return glm::vec3(red / 255.0f, blue / 255.0f, green / 255.0f);
}

float meterToWorldUnit(float meter)
{
	return meter * 10;
}

void calcDirectLightAttrib()
{
	glm::vec3 newColor{};

	glm::vec3 currentDayPhaseColor { World::sunLightColor[Time::dayPhase] };

	DayPhases nextDayPhase{ };
	bool sameDay1{ true };
	bool sameDay2{ true };

	switch (Time::dayPhase)
	{
	case dawn:
		nextDayPhase = daytime;
		break;

	case daytime:
		nextDayPhase = sunset;
		break;

	case sunset:
		nextDayPhase = night;
		break;

	case night:
		nextDayPhase = dawn;
		sameDay1 = false;
		break;
	}

	glm::vec3 nextDayPhaseColor{ World::sunLightColor[nextDayPhase] };


	if (Time::dayPhase == night && Time::timeInGame.hour <= 6)
		sameDay2 = false;

	//linear interpolation of color 
	float currentPhaseNextPhaseDist{ timeToHour( timeDist(Time::dayPhasesTime[nextDayPhase][0],Time::dayPhasesTime[Time::dayPhase][0],sameDay1) ) };
	float currentHourCurrentPhaseBaseHourDist { timeToHour( timeDist(Time::timeInGame,Time::dayPhasesTime[Time::dayPhase][0],sameDay2) ) };

	float currentPhaseHour{ timeToHour(Time::dayPhasesTime[Time::dayPhase][0]) };
	float currentHour { timeToHour(Time::timeInGame) };

	newColor.r = currentDayPhaseColor.r + currentHourCurrentPhaseBaseHourDist * ((nextDayPhaseColor.r - currentDayPhaseColor.r) / currentPhaseNextPhaseDist);
	newColor.g = currentDayPhaseColor.g + currentHourCurrentPhaseBaseHourDist * ((nextDayPhaseColor.g - currentDayPhaseColor.g) / currentPhaseNextPhaseDist);
	newColor.b = currentDayPhaseColor.b + currentHourCurrentPhaseBaseHourDist * ((nextDayPhaseColor.b - currentDayPhaseColor.b) / currentPhaseNextPhaseDist);
	
	World::directLights[0].color = newColor;

}

Time::Time timeDist(Time::Time time1, Time::Time time2,bool sameDay)
{
	Time::Time result;
	
	result.sec =  abs ( time1.sec - time2.sec );
	result.min =  abs ( time1.min - time2.min );

	if (!sameDay)
	{
		result.hour = 24 - time2.hour;
		result.hour += time1.hour;
	}

	else
		result.hour = abs(time1.hour - time2.hour);


	//result.day =  abs ( time1.day - time2.day );

	return result;
}

float timeToHour(Time::Time time)
{
	float hour{};

	//hour = time.day * 24;
	hour += time.hour;
	hour += static_cast<float>(time.min) * (1.0f / 60.0f);
	hour += static_cast<float>(time.sec) * (1.0f / 3600.0f);

	return hour;
}