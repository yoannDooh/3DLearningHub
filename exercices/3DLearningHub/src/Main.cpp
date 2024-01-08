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
	Shader woodBoxShader(".\\shader\\woodBox\\vertex.glsl", ".\\shader\\woodBox\\fragment.glsl");
	Shader lightSourcesShader(".\\shader\\lightSources\\vertex.glsl", ".\\shader\\lightSources\\fragment.glsl");

	float cubeEdge{ 1.0f };
	std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 

	Cube woodCube (cubeEdge, cubeOriginCoord, loadTextures({".\\rsc\\woodContainer.png",".\\rsc\\specularMap.png" ,".\\rsc\\emissionMap.png" }, {diffuse,specular,emission}));

	Cube lightCube;
	lightCube.constructLightCube(woodCube.getVbo(), woodCube.getEbo() );

	//lightCubes 
	light::lightPointCube light;
	std::vector<light::lightPointCube> lightPoints{};
	lightPoints.push_back(light);
	lightPoints.push_back(light);

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

	setLightCube(lightSourcesShader, cubeEdge, lightPoints);
	setLighting(woodBoxShader, lightPoints);
	setWoodCube(woodBoxShader, cubeEdge, lightPoints);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// render loop lambda functions
	auto newFrame = [&window]()
		{
			processInput(window.windowPtr);

			glm::vec3 clearColor{ rgb(29, 16, 10) };
			glClearColor(clearColor.x, clearColor.y, clearColor.z, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		animateWoodCube(woodBoxShader, woodCube, lightPoints);
		animateLightsCube(lightSourcesShader, lightCube, lightPoints);

		swapBuffer();
	}

	glfwTerminate();
	return 0;
}

