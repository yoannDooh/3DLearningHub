#define _USE_MATH_DEFINES
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shaderAndLight.h"
#include "../header/motion.h"
#include "../header/mesh.h"

int main()

{

	glfwWindowHint(GLFW_SAMPLES, 4);
	Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");

	//init shaders
	Shader woodBoxShader(".\\shader\\woodBox\\vertex.glsl", ".\\shader\\woodBox\\fragment.glsl");
	Shader lightSourcesShader(".\\shader\\lightSources\\vertex.glsl", ".\\shader\\lightSources\\fragment.glsl");
	Shader skyboxShader(".\\shader\\skyBox\\vertex.glsl", ".\\shader\\skyBox\\fragment.glsl");
	//Shader outlineShader(".\\shader\\outline\\vertex.glsl", ".\\shader\\outline\\fragment.glsl");

	//woodCube parameters
	float cubeEdge{ 1.0f };
	std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 

	//skybox textures pathes
	std::vector<const char*> skyBoxTextPaths{
			".\\rsc\\skybox\\right.jpg",
			".\\rsc\\skybox\\left.jpg",
			".\\rsc\\skybox\\top.jpg",
			".\\rsc\\skybox\\bottom.jpg",
			".\\rsc\\skybox\\front.jpg",
			".\\rsc\\skybox\\back.jpg"
	};

	//init models
	Cube woodCube (cubeEdge, cubeOriginCoord, loadTextures({".\\rsc\\woodCube\\woodContainer.png",".\\rsc\\woodCube\\specularMap.png" ,".\\rsc\\woodCube\\emissionMap.png" }, {diffuse,specular,emission}),true);
	Cube lightCube(woodCube.getVbo(), woodCube.getEbo());
	CubeMap skyBox (skyBoxTextPaths);

	//lightCubes 
	light::lightPointCube light;
	std::vector<light::lightPointCube> lightPoints{};
	lightPoints.push_back(light);
	lightPoints.push_back(light);

	/*
	//loadCubeMap
	std::vector<const char*> cubeMapFacePathes{
			".\\rsc\\skybox\\right.jpg",
			".\\rsc\\skybox\\left.jpg",
			".\\rsc\\skybox\\top.jpg",
			".\\rsc\\skybox\\bottom.jpg",
			".\\rsc\\skybox\\front.jpg",
			".\\rsc\\skybox\\back.jpg"
	};
	loadCubemap(cubeMapFacePathes);
	*/

	//set models
	setLightCube(lightSourcesShader, cubeEdge, lightPoints);
	setLighting(woodBoxShader, lightPoints);
	setWoodCube(woodBoxShader, cubeEdge, lightPoints);

	//rendering parameters
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_MULTISAMPLE);


	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// render loop lambda functions
	auto newFrame = [&window]()
		{
			processInput(window.windowPtr);

			glm::vec3 clearColor{ rgb(29, 16, 10) };
			glClearColor(clearColor.x, clearColor.y, clearColor.z, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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
		updateViewProject();

		animateLightsCube(lightSourcesShader, lightCube, lightPoints);
		animateWoodCube(woodBoxShader, skyBox.texture.ID, woodCube, lightPoints);

		/*
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		animateWoodCube(woodBoxShader,skyBox.texture.ID,woodCube,lightPoints);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthFunc(GL_ALWAYS);


		outlineShader.use();
		glm::vec3 outlineColor{ rgb(241, 128, 45) };
		outlineShader.set3Float("outLineColor", outlineColor);

		float weight = 0.008f;
		outlineShader.setFloat("outLineWeight",weight);

		outlineShader.setMat4("model", World::woodCube.model);		
		outlineShader.use();
		passViewProject(outlineShader);
		woodCube.draw(outlineShader);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glDepthFunc(GL_LESS);
		*/

		//DRAW IN LAST
		skyBox.draw(skyboxShader);

		swapBuffer();
	}

	glfwTerminate();
	return 0;
}

