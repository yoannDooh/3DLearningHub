#define _USE_MATH_DEFINES

#include "../header/motion.h"
#include <iostream>

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
	float fps;
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
		rgb(226, 239, 245), //color
		{0.391486f, -0.690055f, 0.608738f},//direction
		{ 0.4f, 0.4f, 0.4f,	 },	//ambient
		{ 0.6f, 0.6f, 0.6f,	 },	//diffuse
		{ 1.0f,	 1.0f,  1.0f }, //specular 
		}
	};

	Object woodCube ( glm::vec3(0.0f, 0.0f, 0.0f) );
	std::array<Light::lightPoint, 2> lightCube {};
	std::array<Object, POINT_LIGHTS_NB> lightCubesObject{};


	int mapWidth{};
	int mapHeight{};
}

//OBJECT CLASS
Object::Object(glm::vec3 pos)
{
	model = glm::translate(localOrigin, pos);
	pos = pos;
}

void Object::move(glm::vec3 vector)
{
	model = model*glm::translate(localOrigin, vector);
	pos = glm::translate(localOrigin, vector) * glm::vec4(pos,1.0f);

	//should add a parameter to decide how the light position is influenced by the object it's associated with position 
	//by default the light has the same position as the object 
	//update lightPoint pos
	if (lightPointPtr == nullptr)
		return;

	else
		lightPointPtr->pos = pos;

	//update spotlight pos
	if (spotLightPtr == nullptr)
		return;

	else
		spotLightPtr->pos = pos;
}

void Object::rotate(float rad, glm::vec3 rotateAxis)
{
	model = model * glm::rotate(model, rad, rotateAxis);
	pos = glm::rotate(model, rad, rotateAxis) * glm::vec4(pos, 1.0f);

	if (lightPointPtr == nullptr)
		return;

	else
		lightPointPtr->pos = pos;

	//update spotlight pos
	if (spotLightPtr == nullptr)
		return;

	else
		spotLightPtr->pos = pos;
}

void Object::scale(glm::vec3 scaleVec)
{
	model = model * glm::scale(localOrigin, scaleVec);
	pos = glm::scale(localOrigin, scaleVec) * glm::vec4(pos, 1.0f);

	if (lightPointPtr == nullptr)
		return;

	else
		lightPointPtr->pos = pos;

	//update spotlight pos
	if (spotLightPtr == nullptr)
		return;

	else
		spotLightPtr->pos = pos;
}

void Object::updateLightPoint(glm::vec3 newValue, int memberIndex)
{
	try
	{
		if (lightPointPtr == nullptr)
			throw(-1);

		switch (memberIndex)
		{
		case 0:
			lightPointPtr->color = newValue;
			break;

		case 1:
			lightPointPtr->pos = newValue;
			break;

		case 2:
			lightPointPtr->ambient = newValue;
			break;

		case 3:
			lightPointPtr->diffuse = newValue;
			break;

		case 4:
			lightPointPtr->specular = newValue;
			break;

		}
	}

	catch (const std::exception&)
	{
		std::cerr << "DEREFENCING NULL POINTER WITH animateLightsCube FUNCTION CALL" << std::endl;
	}

}

//shadows functions
glm::mat4 toDirectionalLightSpaceMat(float lightRange, glm::vec3 lightPos, glm::vec3 lookAtLocation)
{
	float near_plane = 1.0f, far_plane = lightRange;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(lookAtLocation), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;			
}

void setupShadowMap(FrameBuffer depthMap, glm::mat4 lightSpaceMat)
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
		genFrameBuffTex(SCR_WIDTH,SCR_HEIGHT,false);

	if (activateRenderBuff)
		genRenderBuff();

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		std::cerr << "renderBUffer failed, error : " << error;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::genFrameBuffTex(int width, int height,bool depthAttachment)
{
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	if (!depthAttachment)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

	}

	else
	{
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

}

void FrameBuffer::genRenderBuff()
{
	glGenRenderbuffers(1, &renderId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderId);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderId);

}

FrameBuffer::FrameBuffer(bool shadowMap)
{
	if (shadowMap)
	{
		SHADOW_WIDTH = 1024;
		SHADOW_HEIGHT = 1024;
		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		genFrameBuffTex(SHADOW_WIDTH, SHADOW_HEIGHT,true);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
			
		if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
		{
			auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
			std::cerr << "renderBuffer failed, error : " << error;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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

//motion functions
void rotatePlane(Object& object, double degree)
{
	degree = degreeToRad(degree);
	glm::mat4 rot{
		glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
				  0.0f, cos(degree), sin(degree), 0.0f,
				  0.0f, -sin(degree), cos(degree), 0.0f,
				  0.0f, 0.0f, 0.0f, 1.0 )  };

	object.localOrigin = rot * object.localOrigin;
	//object.pos = glm::vec3(0.0f, 0.0f, 0.0f); //???
}

glm::mat4 orbit(Object& object, float horizontalAxis, float verticalAxis,float orbitDuration)
{
	glm::mat4 orbit{};
	float xAxisValue{};
	float zAxisValue{};

	bool isRotationclockwise{ true };
	int rotationSens{ 1 }; //1 for clockwise -1 for counter clockwise 

	if (!rotationSens)
		rotationSens = -1;

	if (Time::deltaTime != 0)
	{
		xAxisValue = (horizontalAxis * cos(((2 * M_PI) / orbitDuration) * Time::deltaSum));
		zAxisValue = (verticalAxis * sin(((2 * M_PI) / orbitDuration) * Time::deltaSum));
	}

	orbit = glm::translate(object.localOrigin, glm::vec3(xAxisValue, 0.0f, zAxisValue));

	//object.move(glm::vec3(xAxisValue, 0.0f, zAxisValue) );

	return  orbit;
}

//view & projection function
void updateViewProject()
{
	World::view = glm::lookAt(World::camera.pos, World::camera.pos + World::camera.front, World::camera.up);
	World::projection = glm::perspective(glm::radians(Mouse::fov), World::projectionWidth / World::projectionHeight, World::projectionNear, World::projectionFar);

	fillUbo0(0);
	fillUbo0(1);
}

//EmmisionMap
float frameGlow()
{
	float currentFrameGlow{ };
	const float glowStrenghtMax{ 0.25 }; //max value of glow
	const float durationToReachGlowMax{ 0.9 }; //in sec

	if (Time::deltaSum != 0)
	{
		currentFrameGlow = glowStrenghtMax * sin((M_2_PI / durationToReachGlowMax) * Time::deltaSum);
	}

	if (currentFrameGlow < 0)
		currentFrameGlow *= -1;

	return currentFrameGlow;
}

//LightCube function
void setLightCubes(Shader& shader, float cubeEdge)
{
	shader.use();

	//set lightPoints base model
	int index{};
	for (auto& lightCube : World::lightCubesObject)
	{
		
		lightCube.scale(glm::vec3(0.4f));

		rotatePlane(lightCube, ((index + 1) * 60)); //first is 60° and second 120°
		++index;
	}

	shader.setFloat("cubeEdge", cubeEdge);
	shader.setMat4("model", World::lightCubesObject[0].model); //both have the same model

	//set their setLighting value
	std::array<Light::lightPoint, 2> lights{}; //first is red second is blue 

	for (auto& light: lights)
	{
		light.constant = 1.0f;
		light.linearCoef = 0.0014f;
		light.squareCoef = 0.000007f;

		light.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	lights[0].pos = World::lightCubesObject[0].pos;
	lights[1].pos = World::lightCubesObject[1].pos;

	lights[0].color = rgb(4, 217, 255);
	lights[1].color = rgb(242, 0, 0);

	World::lightPoints.push_back(lights[0]);
	World::lightPoints.push_back(lights[1]);

	World::lightCubesObject[0].lightPointPtr = &World::lightPoints[0];
	World::lightCubesObject[1].lightPointPtr = &World::lightPoints[1];
}

void animateLightsCube(Shader& shader, Cube lightCubeMesh)
{
	glm::mat4 orbitMat{};
	glm::vec3 newPos{};

	shader.use();

	for (auto& const lightCube : World::lightCubesObject)
	{ 
		orbitMat = orbit(lightCube, 4.0f, 4.0f, 15);
		
		//update light pos
		newPos = glm::vec3(orbitMat * glm::vec4(lightCube.pos, 1.0f));

		try
		{
			if (lightCube.lightPointPtr == nullptr)
				throw(-1);

			lightCube.lightPointPtr->pos = newPos;
		}

		catch (const std::exception&)
		{
			std::cerr << "DEREFENCING NULL POINTER WITH animateLightsCube FUNCTION CALL" << std::endl;
		}

		shader.set3Float("lightColor", lightCube.lightPointPtr->color);
		shader.setMat4("model", lightCube.model);
		shader.setMat4("orbit", orbitMat);

		lightCubeMesh.draw(shader);
	}
}

//woodCube function
void setWoodCube(Shader& shader) 
{

	World::woodCube.scale(glm::vec3(10.0f));
	World::woodCube.move(glm::vec3(0.0f, meterToWorldUnit(0.10f), 0.0f));

	shader.use();
	shader.setMat4("model", World::woodCube.model);
	shader.setFloat("material.shininess", 64.0f);
}

void animateWoodCube(Shader& shader,unsigned int cubemapTexture,Cube woodCubeMesh)
{
	shader.use();
	glm::vec3 emmissionColor{ rgb(255, 255, 0) };

	shader.setFloat("emmissionStrength", frameGlow() );
	shader.set3Float("emmissionColor", emmissionColor);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	woodCubeMesh.draw(shader);
}

//POST PROCESSING EFFECT FUNCTION
void animateWoodCubeAndOutline(Shader& woodBoxShader, Shader& outlineShader, unsigned int cubemapTexture, Cube woodCubeVao)
{
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	animateWoodCube(woodBoxShader, cubemapTexture, woodCubeVao);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDepthFunc(GL_ALWAYS);


	outlineShader.use();
	glm::vec3 outlineColor{ rgb(43, 117, 190) };
	outlineShader.set3Float("outLineColor", outlineColor);

	float weight = 0.009f;
	//float weight = 0.006f; is the best but need to find a way to extend the lines
	outlineShader.setFloat("outLineWeight", weight);

	outlineShader.setMat4("model", World::woodCube.model);
	woodCubeVao.draw(outlineShader);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
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

template <typename T> //j'avais zappé y avait glm::radian faudra suppr ça sert à r 
T degreeToRad(T degree)
{
	if (degree > 360 || degree < 0)
	{
		std::cerr << "invalid degree value";
		return -1;
	}

	return degree * (M_PI / 180);
}