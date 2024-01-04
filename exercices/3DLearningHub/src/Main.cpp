#define _USE_MATH_DEFINES
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shader.h"
#include "../header/mesh.h"

int main()
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
	Shader woodBoxShader(".\\shader\\woodBox\\vertex.glsl", ".\\shader\\woodBox\\fragment.glsl");
	Shader lightSourcesShader(".\\shader\\lightSources\\vertex.glsl", ".\\shader\\lightSources\\fragment.glsl");

	glm::vec3 sunLightColor{ rgb(252, 150, 1) };

	light::DirectLight sunLight{
		sunLightColor,
		{-0.2f, -1.0f, -0.3f},
		{ 0.2f, 0.2f, 0.2f,	},
		{ 0.5f, 0.5f, 0.5f,	},
		{ 1.0f,	 1.0f,  1.0f },
	};

	float cubeEdge{ 1.0f };
	std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 

	Cube woodCube (cubeEdge, cubeOriginCoord, loadTextures({".\\rsc\\woodContainer.png",".\\rsc\\specularMap.png" ,".\\rsc\\emissionMap.png" }, {diffuse,specular,emission}));

	Cube lightCube;
	lightCube.constructLightCube(woodCube.getVbo(), woodCube.getEbo() );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);

	// --VARIABLES FOR THE ANIMATION--
	//box glow 
	float glowStrenghtMax{ 0.25 };
	float durationToReachGlowMax{0.9}; //in sec
	float currentFrameGlow{ };

	//Matrixes
	glm::mat4 localOrigin{ glm::mat4(1.0f) };
	glm::mat4 firstLightSourceRotMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(2 * M_PI / 3), sin(2 * M_PI / 3), 0.0f,
		0.0f, -sin(2 * M_PI / 3), cos(2 * M_PI / 3), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 secondLightSourceRotMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(M_PI / 3), sin(M_PI / 3), 0.0f,
		0.0f, -sin(M_PI / 3), cos(M_PI / 3), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);


	glm::mat4 lightSourcelocalOrigin{
		glm::mat4(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,  
		0.0f,0.0f,0.0f,1.0f)
	};

	//lightCubes 
	std::array<light::lightPointCube, 2> lightPoints{};
	glm::mat4 ellipticOrbit{ glm::mat4(1.0f) };

	for (int index{}; index < lightPoints.size(); ++index)
	{

		if (index) //index==1
			lightPoints[index].shiftedLocalOrigin =  firstLightSourceRotMatrix* lightSourcelocalOrigin;

		else
			lightPoints[index].shiftedLocalOrigin = secondLightSourceRotMatrix * lightSourcelocalOrigin ;

		lightPoints[index].pos = glm::vec3(0.0f, 0.0f, 0.0f);
		lightPoints[index].pos = glm::vec3(lightPoints[index].shiftedLocalOrigin * glm::vec4(lightPoints[index].pos,  0.0f));
	}

	lightPoints[0].color = rgb(4, 217, 255);
	lightPoints[1].color = rgb(242, 0, 0);


	lightPoints[0].ambient =  glm::vec3(0.05f, 0.05f, 0.05f);
	lightPoints[0].diffuse =  glm::vec3(0.8f, 0.8f, 0.8f);
	lightPoints[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);

	lightPoints[0].constant = 1.0f;
	lightPoints[0].linearCoef = 0.0014f;
	lightPoints[0].squareCoef = 0.000007f;

	lightPoints[1].constant = 1.0f;
	lightPoints[1].linearCoef = 0.0014f;
	lightPoints[1].squareCoef = 0.000007f;

	lightPoints[1].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	lightPoints[1].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	lightPoints[1].specular = glm::vec3(1.0f, 1.0f, 1.0f);


	//elipses variables    
	float aValue{ 2.0f };
	float bValue{ 2.0f }; //b must be smaller or equal to a
	float xElipse{ aValue };
	float yElipse{ bValue };
	bool isRotationclockwise{ true };
	int rotationSens{ 1 }; //1 for clockwise -1 for counter clockwise 
	if (!rotationSens)
		rotationSens = -1;

	float orbitDuration{ 20 }; //the number of seconds it takes for x to reach the value 2Py (so to do a 360)

	// --WoodBox SHADER-- 
	woodBoxShader.use();

	woodBoxShader.setFloat("cubeEdge", cubeEdge);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "viewPos"), camera.pos.x, camera.pos.y, camera.pos.z);

	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].pos"), lightPoints[0].pos.x, lightPoints[0].pos.y, lightPoints[0].pos.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].pos"), lightPoints[1].pos.x, lightPoints[1].pos.y, lightPoints[1].pos.z);

	//material properties 
	woodBoxShader.setFloat("material.shininess", 64.0f);


	//light properties 
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "sunLight.color"), sunLight.color.x, sunLight.color.y, sunLight.color.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "sunLight.direction"), sunLight.direction.x, sunLight.direction.x, sunLight.direction.x);

	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "sunLight.ambient"), sunLight.ambient.x, sunLight.ambient.y, sunLight.ambient.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "sunLight.diffuse"), sunLight.diffuse.x, sunLight.diffuse.y, sunLight.diffuse.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "sunLight.specular"), sunLight.specular.x, sunLight.specular.y, sunLight.specular.z);


	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].ambient"), lightPoints[0].ambient.x, lightPoints[0].ambient.y, lightPoints[0].ambient.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].diffuse"), lightPoints[0].diffuse.x, lightPoints[0].diffuse.y, lightPoints[0].diffuse.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].specular"), lightPoints[0].specular.x, lightPoints[0].specular.y, lightPoints[0].specular.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].color"), lightPoints[0].color.x, lightPoints[0].color.y, lightPoints[0].color.z);


	woodBoxShader.setFloat("pointLights[0].constant", lightPoints[0].constant);
	woodBoxShader.setFloat("pointLights[0].linearCoef", lightPoints[0].linearCoef);
	woodBoxShader.setFloat("pointLights[0].squareCoef", lightPoints[0].squareCoef);

	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].ambient"), lightPoints[1].ambient.x, lightPoints[1].ambient.y, lightPoints[1].ambient.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].diffuse"), lightPoints[1].diffuse.x, lightPoints[1].diffuse.y, lightPoints[1].diffuse.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].specular"), lightPoints[1].specular.x, lightPoints[1].specular.y, lightPoints[1].specular.z);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].color"), lightPoints[1].color.x, lightPoints[1].color.y, lightPoints[1].color.z);


	woodBoxShader.setFloat("pointLights[1].constant", lightPoints[1].constant);
	woodBoxShader.setFloat("pointLights[1].linearCoef", lightPoints[1].linearCoef);
	woodBoxShader.setFloat("pointLights[1].squareCoef", lightPoints[1].squareCoef);

	//model
	glm::mat4 model{ glm::mat4(1.0f) };
	model = glm::translate(localOrigin, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//view
	glm::mat4 view{ glm::mat4(1.0f) };
	view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
	glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//projection
	glm::mat4 projection{ glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f) };
	glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "objectColor"), 206.0f / 255.0f, 206.0f / 255.0f, 206.0f / 255.0f);
	glUniform3f(glGetUniformLocation(woodBoxShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);


	// --LIGHTSOURCE SHADER -- //
	lightSourcesShader.use();
	lightSourcesShader.setFloat("cubeEdge", cubeEdge);

	//models
	glm::mat4 lightSourceModel{ glm::mat4(1.0f) };
	lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightSourceModel));

	for (int index{}; index < lightPoints.size(); ++index)
	{
		lightPoints[index].model = lightSourceModel;
	}

	//view
	glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//projection
	glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	auto newFrame = []()
		{
			//increment time variables 
			Time::currentFrameTime = glfwGetTime();
			Time::previousDelta = Time::deltaTime;
			Time::deltaTime = Time::currentFrameTime - Time::lastFrameTime;
			Time::lastFrameTime = Time::currentFrameTime;
		};

	auto swapBuffer = [&window]()
		{
			glfwSwapBuffers(window.windowPtr);
			glfwPollEvents();

			//increment time variables 
			if (glfwGetTime() >= Time::sec + 1)
			{
				++Time::sec;
				Time::currentFrame = 1;
			}
			Time::deltaSum += Time::deltaTime;
			++Time::currentFrame;
			++Time::totalFrame;
		};

	// render loop
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window.windowPtr))
	{

		newFrame();
		processInput(window.windowPtr);
		glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glm::vec3 clearColor{ rgb(29, 16, 10) };
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--ANIMATIONS-- 
		if (Time::deltaTime != 0)
		{
			//ellipses 
			xElipse = (aValue * cos(((2 * M_PI) / orbitDuration) * Time::deltaSum));
			yElipse = (bValue * sin(((2 * M_PI) / orbitDuration) * Time::deltaSum));

			//emmisionMap 
			currentFrameGlow = glowStrenghtMax*sin( (M_2_PI / durationToReachGlowMax) * Time::deltaSum);

			if (currentFrameGlow < 0)
			{	
				currentFrameGlow = -glowStrenghtMax * sin((M_2_PI / durationToReachGlowMax) * Time::deltaSum);
			}
		}

		else
		{
			xElipse = (aValue * cos(2 * M_PI));
			yElipse = (bValue * sin(2 * M_PI));
		}

		for (int index{}; index < lightPoints.size(); ++index)
		{
			lightPoints[index].ellipticOrbit = glm::translate(lightPoints[index].shiftedLocalOrigin, glm::vec3(xElipse, 0.0f, yElipse) );
		}

		view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
		projection = glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f);

		//--WoodBox Shader-- 
		woodBoxShader.use();
		glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(woodBoxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//pointLights pos
		for (int index{}; index < lightPoints.size(); ++index)
		{
			lightPoints[index].pos.x = xElipse;

			if (index==0) 
			{
				lightPoints[0].pos.z = secondLightSourceRotMatrix[0][0] * xElipse + secondLightSourceRotMatrix[0][1] * lightPoints[0].pos.y + secondLightSourceRotMatrix[0][2] * yElipse; //je sais pas pourquoi c'est inversé
				glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[0].pos"), lightPoints[0].pos.x, lightPoints[0].pos.y, lightPoints[0].pos.z);
			}

			else
			{
				lightPoints[0].pos.z = firstLightSourceRotMatrix[0][0] * xElipse + firstLightSourceRotMatrix[0][1] * lightPoints[0].pos.y + firstLightSourceRotMatrix[0][2] * yElipse;
				glUniform3f(glGetUniformLocation(woodBoxShader.ID, "pointLights[1].pos"), lightPoints[1].pos.x, lightPoints[1].pos.y, lightPoints[1].pos.z);
			}
		}

		glUniform3f(glGetUniformLocation(woodBoxShader.ID, "viewPos"), camera.pos.x, camera.pos.y, camera.pos.z);
		woodBoxShader.setFloat("emmissionStrength", currentFrameGlow);

		woodCube.draw(woodBoxShader);

		//--LIGHTSOURCE--
		lightSourcesShader.use();
		for (auto& const lightPoint : lightPoints)
		{

			glUniform3f(glGetUniformLocation(lightSourcesShader.ID, "lightColor"), lightPoint.color.x, lightPoint.color.y, lightPoint.color.z);
			glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightPoint.model));
			glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr(lightPoint.ellipticOrbit));
			glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightSourcesShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			lightCube.draw(lightSourcesShader);
		}

		swapBuffer();
	}

	glfwTerminate();
	return 0;
}

