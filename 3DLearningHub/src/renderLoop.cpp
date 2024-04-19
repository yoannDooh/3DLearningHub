#include "../header/renderLoop.h"
#include "../header/tuiWindow.h"
#include <iostream>

void createAndSetLightCube(Shader& shader, std::array<Object, 2>& lightCubesObject)
{
	//create and set lightCube Object

	for (int lightCubeIndex{}; lightCubeIndex < 2; ++lightCubeIndex)
	{

		glm::vec3 color{};
		if (lightCubeIndex == 0)
			color = rgb(4, 217, 255);

		if (lightCubeIndex == 1)
			color = rgb(242, 0, 0);

		lightCubesObject[lightCubeIndex].setLightPoint(color, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0014f, 0.000007f);


		lightCubesObject[lightCubeIndex].setOrbit(1.0, 30.0f, 30.0f, 6);
		lightCubesObject[lightCubeIndex].isOrbiting = true;


		lightCubesObject[lightCubeIndex].addToWorldObjects();
	}

	int index{};
	for (auto& lightCube : lightCubesObject)
	{
		World::objects[lightCube.worldObjIndex].enableTranslation = false;
		World::objects[lightCube.worldObjIndex].enableRotation = false;

		World::objects[lightCube.worldObjIndex].set(shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(0.4f));

		World::objects[lightCube.worldObjIndex].rotatePlane((index + 1) * 60); //first is 60� and second 120�


		++index;
	}
}

void createAndSetWoodCube(Shader& shader, Shader& outlineShader, Object& woodCubeObj)
{
	woodCubeObj.addToWorldObjects();
	woodCubeObj.enableRotation = false;
	woodCubeObj.isOutLined = true;
	woodCubeObj.isGlowing = true;
	woodCubeObj.materialShininess = 64.0f;
	woodCubeObj.shaderOutline = outlineShader;
	woodCubeObj.setGlow(glm::vec3{ rgb(255, 255, 0) }, 0.25, 0.9);
	woodCubeObj.set(shader, glm::vec3(0.0f, meterToWorldUnit(0.1f), 0.0f), glm::vec3(0.0f), 0.0f, glm::vec3(10.0f));
}

void debugInfo()
{
	std::cout << "seconds : " << Time::sec << "\tFrame number : " << Time::totalFrame << "\tFPS : " << Time::fps << "\n";
	std::cout << "\n";

	printTimeInGame();
	printVec3("cameraPos", World::camera.pos);
	printLine(50);
	std::cout << "\n";
}

void checkUsrRenderParameter(Shader& postProcessShader)
{
	//effect ? 
	setEffect(postProcessShader, UsrParameters::currentEffect);

	//wireframe ? 
	if (UsrParameters::activateWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}