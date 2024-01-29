#include <iostream>
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shaderAndLight.h"
#include "../header/motion.h"
#include "../header/mesh.h"

std::vector<float>points  {
-0.5f, 0.5f,0.0f, // top-left
0.5f, 0.5f,0.0f, // top-right
0.5f, -0.5f,0.0f, // bottom-right
-0.5f, -0.5f, 0.0f,// bottom-left
};

std::vector<float>circleCenter{
	0.0f, 0.0f,0.0f, 
};

int main()
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");

	//init shaders
	Shader woodBoxShader(".\\shader\\woodBox\\vertex.glsl", ".\\shader\\woodBox\\fragment.glsl");
	Shader lightSourcesShader(".\\shader\\lightSources\\vertex.glsl", ".\\shader\\lightSources\\fragment.glsl");
	Shader skyboxShader(".\\shader\\skyBox\\vertex.glsl", ".\\shader\\skyBox\\fragment.glsl");
	Shader outlineShader(".\\shader\\outline\\vertex.glsl", ".\\shader\\outline\\fragment.glsl");
	Shader postProcessShader(".\\shader\\postProcess\\vertex.glsl", ".\\shader\\postProcess\\fragment.glsl");
	Shader geometryShader(".\\shader\\house\\vertex.glsl", ".\\shader\\house\\fragment.glsl", ".\\shader\\house\\geometry.glsl");
	Shader circleShader (".\\shader\\circle\\vertex.glsl", ".\\shader\\circle\\fragment.glsl", ".\\shader\\circle\\geometry.glsl");
	Shader terrainShader (".\\shader\\terrain\\vertex.glsl", ".\\shader\\terrain\\fragment.glsl", ".\\shader\\terrain\\TCS.glsl", ".\\shader\\terrain\\TES.glsl");
	Shader simpleShadowShader(".\\shader\\simpleShadowMap\\vertex.glsl", ".\\shader\\simpleShadowMap\\fragment.glsl");


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
	Terrain terrain(8,".\\rsc\\terrain\\drole.png");

	//square for postProcess
	std::array<float, 2> origin{ 1.0f,1.0f };
	Square quad(2.0, origin);


	//quad for geometry Shader 
	Points quadPoints(points);
	Points circle(circleCenter);


	//lightCubes 
	light::lightPointCube light;
	std::vector<light::lightPointCube> lightPoints{};
	lightPoints.push_back(light);
	lightPoints.push_back(light);

	//set models
	setLightCube(lightSourcesShader, cubeEdge, lightPoints);
	setLighting(woodBoxShader, lightPoints);
	setWoodCube(woodBoxShader, lightPoints);

	//rendering parameters
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_CLAMP);
	glPatchParameteri(GL_PATCH_VERTICES, 4);


	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	FrameBuffer fbo(true, true);

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

	auto drawHouse = [&geometryShader, &quadPoints]()
		{
			geometryShader.use();
			quadPoints.draw();
		};

	auto drawCircle = [&circleShader, &circle]()
		{
			circleShader.use();
			circleShader.setFloat("radius", 0.3f);
			circleShader.setInt("pointsNb", 85);
			circle.draw();
		};

	auto drawSceneWithEffect = [&fbo,&lightSourcesShader,&lightCube,&lightPoints,&woodBoxShader,&outlineShader,&skyBox,&woodCube,&skyboxShader,&quad,&postProcessShader,&newFrame]()
		{
			//BindFbo
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			newFrame();

			//DrawScene
			updateViewProject();
			animateLightsCube(lightSourcesShader, lightCube, lightPoints);
			animateWoodCubeAndOutline(woodBoxShader, outlineShader, skyBox.texture.ID, woodCube, lightPoints);
			//DRAW IN LAST
			skyBox.draw(skyboxShader);

			//drawTexture Fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			quad.draw(postProcessShader, "screenTexture", fbo.texId);

		};

	auto drawScene = [&fbo, &lightSourcesShader, &lightCube, &lightPoints, &woodBoxShader, &outlineShader, &skyBox, &woodCube, &skyboxShader]()
		{
			//DrawScene
			updateViewProject();
			animateLightsCube(lightSourcesShader, lightCube, lightPoints);
			animateWoodCubeAndOutline(woodBoxShader, outlineShader, skyBox.texture.ID, woodCube, lightPoints);

			//DRAW IN LAST
			skyBox.draw(skyboxShader);

		};

	glm::mat4 model{ glm::mat4(1.0f) };
	auto drawTerrain = [&terrainShader,&terrain,&skyBox,&skyboxShader,&model]()
		{

			terrainShader.use();
			updateViewProject();
			passViewProject(terrainShader);
			terrainShader.set3Float("viewPos", World::camera.pos);
			terrainShader.setMat4("model", model);
			terrainShader.setFloat("maxDistLod", 3000);

			terrain.draw(terrainShader);

			//DRAW IN LAST
			//skyBox.draw(skyboxShader);

		};

	Texture shadowMap;
	FrameBuffer depthMap(true);
	glm::mat4 lightSpaceMat(toDirectionalLightSpaceMat(7.5f, glm::vec3(0.11236f, 1.28743f, -1.49997f), glm::vec3(0.0f)));
	auto drawShadow = [&simpleShadowShader,&depthMap,&shadowMap,&lightSpaceMat,&lightPoints,&woodCube,&woodBoxShader,&drawScene,&terrainShader,&drawTerrain]()
		{
			//pass uniforms to  draw
			setWoodCube(simpleShadowShader, lightPoints);
			passViewProject(simpleShadowShader);

			//setup viewPort size dans fbo
			setupShadowMap(simpleShadowShader,depthMap, lightSpaceMat);
		
			//draw to fbo
			woodCube.draw(simpleShadowShader);

			woodBoxShader.use();
			woodBoxShader.setMat4("lightSpaceMat", lightSpaceMat);

			shadowMap.ID = depthMap.texId;
			shadowMap.type = TextureMap::shadowMap;
			woodCube.addTexture(shadowMap);
			
			terrainShader.use();
			terrainShader.setInt("shadowMap", depthMap.texId);


			//render scene as usual
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			drawScene();
			drawTerrain();
				

		};

	//setEffect(postProcessShader, edgeDetection);


	//wireframe On
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window.windowPtr))
	{
		
		newFrame();

		drawTerrain();

		swapBuffer();
	}

	glfwTerminate();
	return 0;
}