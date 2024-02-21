#include <iostream>
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shader.h"
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

void printVec3(std::string_view objectName, glm::vec3 object);
void printLine(int dashNb);

 int main()
 {
	glfwWindowHint(GLFW_SAMPLES, 4);
	Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");

	//init shaders 
    Shader objectShader(".\\shader\\object\\vertex.glsl", ".\\shader\\object\\fragment.glsl");
	Shader objectDirectShadowShader(".\\shader\\objectDirectShadow\\vertex.glsl", ".\\shader\\objectDirectShadow\\fragment.glsl");
	Shader objectPointShadowShader(".\\shader\\objectPointShadow\\vertex.glsl", ".\\shader\\objectPointShadow\\fragment.glsl", ".\\shader\\objectPointShadow\\geometry.glsl");
	Shader lightSourcesShader(".\\shader\\lightSources\\vertex.glsl", ".\\shader\\lightSources\\fragment.glsl");
	Shader skyboxShader(".\\shader\\skyBox\\vertex.glsl", ".\\shader\\skyBox\\fragment.glsl");
	Shader outlineShader(".\\shader\\outline\\vertex.glsl", ".\\shader\\outline\\fragment.glsl");
	Shader postProcessShader(".\\shader\\postProcess\\vertex.glsl", ".\\shader\\postProcess\\fragment.glsl");
	Shader geometryShader(".\\shader\\house\\vertex.glsl", ".\\shader\\house\\fragment.glsl", ".\\shader\\house\\geometry.glsl");
	Shader circleShader (".\\shader\\circle\\vertex.glsl", ".\\shader\\circle\\fragment.glsl", ".\\shader\\circle\\geometry.glsl");
	Shader terrainShader (".\\shader\\terrain\\vertex.glsl", ".\\shader\\terrain\\fragment.glsl", ".\\shader\\terrain\\TCS.glsl", ".\\shader\\terrain\\TES.glsl");
	Shader terrainDirectShadowShader(".\\shader\\terrainDirectShadow\\vertex.glsl", ".\\shader\\terrainDirectShadow\\fragment.glsl", ".\\shader\\terrainDirectShadow\\TCS.glsl", ".\\shader\\terrainDirectShadow\\TES.glsl");
	Shader jsp(".\\shader\\jsp\\vertex.glsl", ".\\shader\\jsp\\fragment.glsl");



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
	Cube woodCube (cubeEdge, cubeOriginCoord, loadTextures({".\\rsc\\woodCube\\woodContainer.png",".\\rsc\\woodCube\\specularMap.png" ,".\\rsc\\woodCube\\emissionMap.png" }, {diffuse,specular,emission}) );
	woodCube.activateCubeMap = true;
	woodCube.activateShadow = true;

	Cube lightCube(woodCube.getVbo(), woodCube.getEbo());
	CubeMap skyBox (skyBoxTextPaths);

	woodCube.addTexture(skyBox.texture);

	Terrain terrain(2,".\\rsc\\terrain\\heightMaps\\drole.png");
	Icosahedron icosahedron(1.0f,glm::vec3(0.0f,0.0f,0.0f));

	//square for postProcess
	std::array<float, 2> origin{ 1.0f,1.0f };
	Square quad(2.0, origin);


	//quad for geometry Shader 
	Points quadPoints(points);
	Points circle(circleCenter);

	//rendering parameters
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_CLAMP);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	//init Uniforms buffer 
	genUbo0();
	genUbo1();
	genUbo2();

	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set models
	setLightCubes(lightSourcesShader, cubeEdge);
	setWoodCube(objectShader);
	terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\brickWall\\diffuseMap.jpg",".\\rsc\\terrain\\brickWall\\normalMap.jpg" }, { diffuse,normal }), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addChunk(0, north, 2, ".\\rsc\\terrain\\heightMaps\\heightMap2.jpeg");
	//terrain.addChunk(1, west, 2, ".\\rsc\\terrain\\heightMaps\\b.jpg");


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

	auto drawScene = [&fbo, &lightSourcesShader, &lightCube, &objectShader, &outlineShader, &skyBox, &woodCube, &skyboxShader]()
		{
			//DrawScene
			setLighting();
			updateViewProject();
			animateLightsCube(lightSourcesShader, lightCube);
			animateWoodCubeAndOutline(objectShader, outlineShader, woodCube);

			//DRAW IN LAST
			skyBox.draw(skyboxShader);

		};

	auto drawSceneWithEffect = [&drawScene,&fbo, &lightSourcesShader, &lightCube, &objectShader, &outlineShader, &skyBox, &woodCube, &skyboxShader, &quad, &postProcessShader, &newFrame]()
		{
			//BindFbo
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			newFrame();

			//DrawScene
			drawScene();

			//DRAW IN LAST
			skyBox.draw(skyboxShader);

			//drawTexture Fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			quad.draw(postProcessShader, "screenTexture", fbo.texId);

		};

	glm::mat4 model{ glm::mat4(1.0f) };
	auto drawTerrain = [&terrainShader,&terrain,&skyBox,&skyboxShader,&model]()
		{
			float a{ 1.0f / static_cast<float>(terrain.width) };
			float b{ 1.0f / static_cast<float>(terrain.height) };

			terrainShader.use();
			terrainShader.set3Float("viewPos", World::camera.pos);
			terrainShader.setMat4("model", model);
			terrainShader.setFloat("maxDistLod", 3000);


			terrainShader.setFloat("area1.shininess", 512.0f);
			terrainShader.setFloat("area1.specularIntensity", 0.0f);


			setLighting();
			terrain.draw(terrainShader);

			//DRAW IN LAST
			skyBox.draw(skyboxShader);

		};

	Texture shadowMap;
	shadowMap.type = TextureMap::shadowMap;

	Texture cubeShadowMap;
	cubeShadowMap.type = TextureMap::shadowCubeMap;

	ShadowBuffer depthMap;
	depthMap.genDepthMapBuff();
	depthMap.genDepthMapLightSpaceMat(10000.0f, glm::vec3(-2532.0f, 7877.0f, -2065.0f), glm::vec3(0.0f));

	ShadowBuffer cubeDepthMap;
	cubeDepthMap.genCubeMapBuff();

	auto drawShadow = [&model,&objectDirectShadowShader,&terrainDirectShadowShader,&depthMap,&shadowMap,&woodCube,&objectShader,&drawScene,&terrainShader,&drawTerrain,&terrain]()
		{

			//lightSpaceMat = toDirectionalLightSpaceMat(1000.0f,World::camera.pos, glm::vec3(0.0f) );
			
			//setup viewPort size dans fbo
			setupShadowMap(depthMap);
			glCullFace(GL_FRONT);

			//pass uniforms to objectDirectShadowShader
			objectDirectShadowShader.use();
			objectDirectShadowShader.setMat4("model", World::woodCube.model);
			objectDirectShadowShader.setMat4("lightSpaceMat", depthMap.depthMapLightSpaceMat);
			woodCube.draw(objectDirectShadowShader);
			

			//pass uniforms to terrainDirectShadowShader
			
			terrainDirectShadowShader.use();
			terrainDirectShadowShader.setMat4("model", model);
			terrainDirectShadowShader.setMat4("lightSpaceMat", depthMap.depthMapLightSpaceMat);
			terrain.drawChunk(0, terrainDirectShadowShader);
			
			glCullFace(GL_BACK);


			objectShader.use();
			objectShader.setMat4("lightSpaceMat", depthMap.depthMapLightSpaceMat);

			terrainShader.use();
			terrainShader.setMat4("lightSpaceMat", depthMap.depthMapLightSpaceMat);

			shadowMap.ID = depthMap.texId;
			woodCube.addTexture(shadowMap);
			terrain.addShadowMap(0, shadowMap);
			 
			//render scene as usual
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			drawScene();
			drawTerrain();
		};

	auto drawPointShadow = [&cubeDepthMap,&objectPointShadowShader,&woodCube,&lightSourcesShader,&lightCube,&cubeShadowMap,&drawScene,&drawTerrain,&terrain,&terrainShader]()
		{

			setLighting();

			//animateLightsCube(lightSourcesShader, lightCube);
			cubeDepthMap.genCubeMapLightSpaceMat(25.0f, World::lightPoints[0].pos);

			//setup viewPort size dans fbo
			setupShadowMap(cubeDepthMap);
			glCullFace(GL_FRONT);

			//pass uniforms to objectPointShadowShader
			objectPointShadowShader.use();
			objectPointShadowShader.setMat4("model", World::woodCube.model);
			objectPointShadowShader.set3Float("lightPos", World::lightPoints[0].pos);
			objectPointShadowShader.setFloat("farPlane",25.0f);

			//pass to it lightSpaceMat
			std::string name{};
			for (int matIndex{}; matIndex < 6; ++matIndex)
			{
				name = "pointShadowLightSpaceMat[";
				name += std::to_string(matIndex);
				name += "]";

				objectPointShadowShader.setMat4(name,cubeDepthMap.cubeMapLightSpaceMat[matIndex]);
			}

			woodCube.draw(objectPointShadowShader);

			cubeShadowMap.ID = cubeDepthMap.texId;
			woodCube.addTexture(cubeShadowMap);
			terrain.addCubeShadowMap(0, cubeShadowMap);

			terrainShader.use();
			terrainShader.setFloat("pointLightFarPlane", 25.0f);
		
			//render scene as usual
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			drawScene();
			drawTerrain();
		};

	auto debugInfo = []()
		{
			std::cout <<"seconds : " << Time::sec << "\tFrame number : " << Time::totalFrame << "\tFPS : " << Time::fps << "\n";
			printLine(50);
			std::cout << "\n";
			printVec3("cameraPos", World::camera.pos);
		};

	auto swapBuffer = [&window,&debugInfo]()
		{
			glfwSwapBuffers(window.windowPtr);
			glfwPollEvents();

			//increment time variables and debugInfo
			if (glfwGetTime() >= Time::sec + 1)
			{
				++Time::sec;
				Time::currentFrame = 1;
				Time::fps = static_cast<float>(Time::totalFrame) / static_cast<float>(Time::sec);
				debugInfo();
			}
			Time::deltaSum += Time::deltaTime;
			++Time::currentFrame;
			++Time::totalFrame;
		};

	glm::mat4 modele{ glm::mat4(1.0f) };
	auto drawIcosahedron = [&skyboxShader, &modele, &jsp, &icosahedron, &skyBox]()
		{
			updateViewProject();
			jsp.use();
			jsp.setMat4("model", modele);
			icosahedron.draw();
			skyBox.draw(skyboxShader);
		};

	//setEffect(postProcessShader, edgeDetection);

	//wireframe On
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window.windowPtr))
	{	
		newFrame();
		//drawPointShadow();
		drawIcosahedron();
	
		swapBuffer();
	}

	glfwTerminate();
 	return 0;
}

void printVec3(std::string_view objectName,glm::vec3 object)
{
	std::cout << objectName << ":" << '\n';

	std::cout << '\t' << "x VALUE : "<< object.x << "\t(in world space unit : " << object.x/10.0f << ')'<<'\n';
	std::cout << '\t' << "y VALUE : "<< object.y << "\t(in world space unit : " << object.y/10.0f << ')'<< '\n';
	std::cout << '\t' << "z VALUE : "<< object.z << "\t(in world space unit : " << object.z/10.0f << ')'<< '\n';
	std::cout << "\n\n";
}

void printLine(int dashNb)
{
	for (int count{};count<= dashNb; ++count)
	{
		std::cout << '-';
	}
}