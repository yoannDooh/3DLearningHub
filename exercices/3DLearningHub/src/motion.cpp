#define _USE_MATH_DEFINES

#include "../header/motion.h"
#include <iostream>

namespace Mouse
{
	float sensitivity = 0.1f;
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
}

namespace lightVar
{
	//sunlight Ambient 
	glm::vec3 sunLightColor { rgb(226, 239, 245) };
	light::DirectLight sunLight{	
		sunLightColor,
		{-0.2f, -1.0f, -0.3f },	//direction
		{ 0.4f, 0.4f, 0.4f,	 },	//ambient
		{ 0.5f, 0.5f, 0.5f,	 },	//diffuse
		{ 1.0f,	 1.0f,  1.0f }, //specular
	};
}

namespace World
{
	Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f };
	glm::mat4 view { glm::lookAt(camera.pos, camera.pos + camera.front, camera.up) };
	glm::mat4 projection {  glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f)  };
	Object::Model woodCube{};
}

//frameBuffer CLASS
FrameBuffer::FrameBuffer(bool activateBufferTex, bool activateRenderBuff)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (activateBufferTex)
		genFrameBuffTex();

	if (activateRenderBuff)
		genRenderBuff();

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		auto error{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		std::cerr << "renderBUffer failed, error : " << error;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::genFrameBuffTex()
{
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

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

void setEffect(Shader& shader,Effects effect)
{
	shader.use();
	shader.setInt("effectIndex", effect);
}


//LIGHT SHOULD USE A MODEL STRUCT 

//Animation functions 
float frameGlow()
{
	float currentFrameGlow{ };
	const float glowStrenghtMax{ 0.25 }; //max value of glow
	const float durationToReachGlowMax{ 0.9 }; //in sec

	if (Time::deltaSum!=0)
	{
		currentFrameGlow = glowStrenghtMax * sin((M_2_PI / durationToReachGlowMax) * Time::deltaSum);
	}

	if (currentFrameGlow < 0)
		currentFrameGlow*=-1;

	return currentFrameGlow;
}

void rotatePlane(light::lightPointCube& light,int index) //rotate plane and add 60�*index+1 to the rotaion index being the index of the light if the array 
{	
	
	//for now don't have an array of more than 6 element
	light.localOrigin = glm::mat4(1.0f);

	glm::mat4 rot{};
	double rad{};
	
	switch (index)
	{
	case 0:
		rad = M_PI / 3;
		break;

	case 1:
		rad = 2*M_PI / 3;
		break;

	case 2:
		rad = M_PI;
		break;

	case 3:
		rad = 4*M_PI/ 3;
		break;

	case 4:
		rad = 5*M_PI / 3;
		break;

	case 5:
		rad = 0;
		break;
	}
	
	rot = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(rad), sin(rad), 0.0f,
		0.0f, -sin(rad), cos(rad), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);


	light.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	light.localOrigin = rot * light.localOrigin;


	light.pos = glm::vec3(light.localOrigin * glm::vec4(light.pos, 0.0f));

}

glm::mat4 orbit(light::lightPointCube& light) //the elipse has world x axis for x axis and world z axis for y axis
{
	glm::mat4 orbit{};
	float horizontalAxis{ 2.0f };
	float verticalAxis{ 2.0f }; //b must be smaller or equal to a

	float xAxisValue{  };
	float zAxisValue{  };

	bool isRotationclockwise{ true };
	int rotationSens{ 1 }; //1 for clockwise -1 for counter clockwise 

	if (!rotationSens)
		rotationSens = -1;


	float orbitDuration{ 20 }; //the number of seconds it takes for x to reach the value 2Py (so to do a 360)

	if (Time::deltaTime != 0)
	{
		xAxisValue = (horizontalAxis * cos(((2 * M_PI) / orbitDuration) * Time::deltaSum));
		zAxisValue = (verticalAxis * sin(((2 * M_PI) / orbitDuration) * Time::deltaSum));
	}

	orbit = glm::translate(light.localOrigin, glm::vec3(xAxisValue, 0.0f, zAxisValue));

	/*
	//update lightPosition
	light.pos = glm::vec4(orbit * glm::vec4(light.pos, 1.0f));
	*/

	return  orbit;
}

void setLightCube(Shader& shader, float cubeEdge, std::vector<light::lightPointCube>& lightCubes)
 {
	shader.use();

	//lightPoints
	shader.setFloat("cubeEdge", cubeEdge);

	//set lightPoints base model
	glm::mat4 lightSourceModel{ glm::mat4(1.0f) };
	lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightSourceModel));

	int index{};
	for (auto& lightCube : lightCubes)
	{
		rotatePlane(lightCube, index);
		lightCube.model = lightSourceModel;
		++index;
	}

}

void setLighting(Shader& shader,std::vector<light::lightPointCube>& lightCubes)
{
	//fill lightCubes values
	for (auto& lightCube : lightCubes )
	{
		lightCube.constant = 1.0f;
		lightCube.linearCoef = 0.0014f;
		lightCube.squareCoef = 0.000007f;

		lightCube.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		lightCube.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		lightCube.specular = glm::vec3(1.0f, 1.0f, 1.0f);

		lightCube.color = rgb(250, 208, 130);
	}

	//suppose that there is only 2 cube for now 
	lightCubes[0].color = rgb(4, 217, 255);
	lightCubes[1].color = rgb(242, 0, 0);

	/*PASS UNIFORMS*/
	shader.use();

	//directional light
	shader.set3Float("sunLight.color",lightVar::sunLight.color);
	shader.set3Float("sunLight.direction", lightVar::sunLight.direction);
	shader.set3Float("sunLight.ambient", lightVar::sunLight.ambient);
	shader.set3Float("sunLight.diffuse", lightVar::sunLight.diffuse);
	shader.set3Float("sunLight.specular", lightVar::sunLight.specular);

	
	//set each lightPoints its attributes
	int index{}; //THERE ARE ONLY 2 LIGHTPOINTS DON'T FORGET TO CHANGE SHADER IF YOU WANT TO ADD MORE

	std::array<std::string, 7> attributes{
		"ambient",
		"diffuse",
		"specular",
		"color",
		"constant",
		"linearCoef",
		"squareCoef",
	};
	for (const auto& lightCube:lightCubes)
	{		
		int index2{};
		for (const auto& attribute : attributes)
		{
			std::string name{ "pointLights[" };
			name += std::to_string(index);
			name += "].";
			name += attribute;

			if (index2 < 4)
			{
				glm::vec3 value;
				switch (index2)
				{
					case 0:
						value = lightCube.ambient;
						break;

					case 1:
						value = lightCube.diffuse;
						break;

					case 2:
						value = lightCube.specular;
						break;

					case 3:
						value = lightCube.color;
						break;
				}
				shader.set3Float(name, value);
			}
			
			else
			{
				float value;
				switch (index2)
				{
					case 4:
						value = lightCube.constant;
						break;

					case 5:
						value = lightCube.linearCoef;
						break;

					case 6:
						value = lightCube.squareCoef;
						break;
				}
				shader.setFloat(name, value);
			}

			++index2;
		}
		++index;
	}
}

void animateLightsCube(Shader& shader,Cube lightCubeVao,std::vector<light::lightPointCube>& lightCubes)
{
	shader.use();
	for (auto& const lightCube : lightCubes)
	{
	
		glUniform3f(glGetUniformLocation(shader.ID, "lightColor"), lightCube.color.x, lightCube.color.y, lightCube.color.z);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightCube.model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr( orbit(lightCube) ) );

		passViewProject(shader);
		lightCubeVao.draw(shader);	
	}
}

void setWoodCube(Shader& shader,float cubeEdge, std::vector<light::lightPointCube>& lightCubes)
{
	World::woodCube.model = glm::translate(World::woodCube.localOrigin, glm::vec3(0.0f, 0.0f, 0.0f));
	World::woodCube.model = glm::rotate(World::woodCube.model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	shader.use();
	shader.setMat4("model", World::woodCube.model);
	shader.setFloat("cubeEdge", cubeEdge);
	shader.setFloat("material.shininess", 64.0f);

	//only 2 for now
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].pos"), lightCubes[0].pos.x, lightCubes[0].pos.y, lightCubes[0].pos.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].pos"), lightCubes[1].pos.x, lightCubes[1].pos.y, lightCubes[1].pos.z);
	
}

void animateWoodCube(Shader& shader,unsigned int cubemapTexture,Cube woodCubeVao ,std::vector<light::lightPointCube>& lightCubes)
{
	//updateLightPos
	//incompleted ?
	shader.use();
	glm::vec3 emmissionColor{ rgb(255, 255, 0) };

	int index{};
	for (auto& lightPoint : lightCubes)
	{

		glm::vec3 newPos{ glm::vec4(orbit(lightPoint) * glm::vec4(lightPoint.pos, 1.0f)) };

		if (index == 0)
			shader.set3Float("pointLights[0].pos", newPos);

		else
			shader.set3Float("pointLights[1].pos", newPos);
	
		++index;
	}

	shader.setFloat("emmissionStrength", frameGlow() );
	shader.set3Float("emmissionColor", emmissionColor);
	shader.set3Float("viewPos", World::camera.pos);


	passViewProject(shader);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	woodCubeVao.draw(shader);
}

void updateViewProject()
{
	World::view = glm::lookAt(World::camera.pos, World::camera.pos + World::camera.front, World::camera.up);
	World::projection = glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f);
}

void passViewProject(Shader& shader)
{
	shader.setMat4("view", World::view);
	shader.setMat4("projection", World::projection);
} 

/*POST PROCESSING EFFECT*/
void animateWoodCubeAndOutline(Shader& woodBoxShader, Shader& outlineShader, unsigned int cubemapTexture, Cube woodCubeVao, std::vector<light::lightPointCube>& lightCubes)
{
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	animateWoodCube(woodBoxShader, cubemapTexture, woodCubeVao, lightCubes);

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
	passViewProject(outlineShader);
	woodCubeVao.draw(outlineShader);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
}

