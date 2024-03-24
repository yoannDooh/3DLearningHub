#include <iostream>
#include <thread>
#include <windows.h>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shader.h"
#include "../header/motion.h"
#include "../header/mesh.h"
#include "../header/tuiWindow.h"


std::vector<float>points  {
-0.5f, 0.5f,0.0f, // top-left
0.5f, 0.5f,0.0f, // top-right
0.5f, -0.5f,0.0f, // bottom-right
-0.5f, -0.5f, 0.0f,// bottom-left
};

std::vector<float>circleCenter{
	0.0f, 0.0f,0.0f, 
};

void createAndSetLightCube(Shader& shader, std::array<Object, 2>& lightCubesObject);
void createAndSetWoodCube(Shader& shader, Shader& outlineShader, Object& woodCubeObj);

 int main()
 {

	glfwWindowHint(GLFW_SAMPLES, 8);
	Window window(2,2,"3DLearningHub");

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
	Shader passThroughShader(".\\shader\\passThrough\\vertex.glsl", ".\\shader\\passThrough\\fragment.glsl");
	Shader sphereShader(".\\shader\\sphere\\vertex.glsl", ".\\shader\\sphere\\fragment.glsl", ".\\shader\\sphere\\TCS.glsl", ".\\shader\\sphere\\TES.glsl");
	Shader hemisphereShader(".\\shader\\hemisphere\\vertex.glsl", ".\\shader\\hemisphere\\fragment.glsl", ".\\shader\\hemisphere\\TCS.glsl", ".\\shader\\hemisphere\\TES.glsl");
	Shader cloudShader(".\\shader\\clouds\\vertex.glsl", ".\\shader\\clouds\\fragment.glsl");

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

	Cube lightCube( woodCube.getVbo(), woodCube.getEbo(),36 );
	CubeMap skyBox (skyBoxTextPaths);

	woodCube.addTexture(skyBox.texture);

	Terrain terrain(2,".\\rsc\\terrain\\heightMaps\\drole.png");
	Icosahedron icosahedron(1.0f,glm::vec3(0.0f,0.0f,0.0f));

	std::array<Object, 2> lightCubesObject{};
	Object woodCubeObj;

	//square of screenSize for postProcess
	std::array<float, 2> origin{ 1.0f,1.0f };
	Square quad(2.0, origin);

	//square for cloud texture
	Square cloudQuad(2.0, origin, loadTexture(".\\rsc\\skydome\\cloud2.png", diffuse));


	CubeForHemisphere hemisphere;

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

	//init Uniforms buffer 
	genUbo0();
	genUbo1();
	genUbo2();

	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window.windowPtr, window_size_callback);


	//set models	
	createAndSetWoodCube(objectShader, outlineShader, woodCubeObj);
	createAndSetLightCube(lightSourcesShader,lightCubesObject);

	//terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\brickWall\\diffuseMap.jpg",".\\rsc\\terrain\\brickWall\\normalMap.jpg" }, { diffuse,normal }), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\coralStoneWall\\diffuseMap.jpg",".\\rsc\\terrain\\coralStoneWall\\normalMap.jpg",".\\rsc\\terrain\\coralStoneWall\\displacementMap.jpg" }, { diffuse,normal,displacement}), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addChunk(0, north, 2, ".\\rsc\\terrain\\heightMaps\\heightMap2.jpeg");
	//terrain.addChunk(1, west, 2, ".\\rsc\\terrain\\heightMaps\\b.jpg");

	FrameBuffer fbo(true, true);
	FrameBuffer skyFbo(true, true);

	Time::timeAccelerator = 50.0f;

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

	auto drawScene = [&fbo, &lightSourcesShader, &lightCube, &objectShader, &outlineShader, &skyBox, &woodCube, &skyboxShader,&lightCubesObject,&woodCubeObj]()
		{
			//DrawScene
			setLighting();
			updateViewProject();
			
			for (auto& lightCubeObj : lightCubesObject)
			{
				World::objects[lightCubeObj.worldObjId].enableTranslation = false;
				World::objects[lightCubeObj.worldObjId].enableRotation = false;
				World::objects[lightCubeObj.worldObjId].enableScale = false;


				World::objects[lightCubeObj.worldObjId].animate(lightCube, lightSourcesShader,glm::vec3(0.0, 0.0, 0.0f),glm::vec3(0.0, 0.0, 0.0f), 0.0f,glm::vec3(0.0,0.0,0.0f) );
			}
			
			//animateWoodCubeAndOutline(objectShader, outlineShader, woodCube);
			woodCubeObj.enableScale = false;
			woodCubeObj.enableRotation = false;
			woodCubeObj.enableTranslation = false;
			woodCubeObj.animate(woodCube, objectShader, glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, glm::vec3(0.0f));

			//DRAW IN LAST
			//skyBox.draw(skyboxShader);

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
			terrainShader.setMat4("model", model);
			terrainShader.setFloat("maxDistLod", 300);


			terrainShader.setFloat("area1.shininess", 512.0f);
			terrainShader.setFloat("area1.specularIntensity", 0.0f);


			setLighting();
			terrain.draw(terrainShader);

			//DRAW IN LAST
			//skyBox.draw(skyboxShader);

		};

	Texture shadowMap;
	shadowMap.type = TextureMap::shadowMap;

	Texture cubeShadowMap;
	cubeShadowMap.type = TextureMap::shadowCubeMap;

	ShadowBuffer depthMap;
	depthMap.genDepthMapBuff();
	//depthMap.genDepthMapLightSpaceMat(1000.0f, glm::vec3(-2532.0f, 7877.0f, -2065.0f), glm::vec3(0.0f));

	ShadowBuffer cubeDepthMap;
	cubeDepthMap.genCubeMapBuff();

	auto drawShadow = [&model,&objectDirectShadowShader,&terrainDirectShadowShader,&depthMap,&shadowMap,&woodCube,&objectShader,&drawScene,&terrainShader,&drawTerrain,&terrain,&woodCubeObj]()
		{

			//lightSpaceMat = toDirectionalLightSpaceMat(1000.0f,World::camera.pos, glm::vec3(0.0f) );
			

			depthMap.genDepthMapLightSpaceMat(1000.0f,glm::vec3(500*World::sunPos.x, 500*World::sunPos.y, 500*World::sunPos.z), glm::vec3(0.0f));

			//setup viewPort size dans fbo
			setupShadowMap(depthMap);
			glCullFace(GL_FRONT);

			//pass uniforms to objectDirectShadowShader
			objectDirectShadowShader.use();
			objectDirectShadowShader.setMat4("model", woodCubeObj.model);
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

	auto drawPointShadow = [&cubeDepthMap,&objectPointShadowShader,&woodCube,&lightSourcesShader,&lightCube,&cubeShadowMap,&drawScene,&drawTerrain,&terrain,&terrainShader,&woodCubeObj]()
		{

			setLighting();

			cubeDepthMap.genCubeMapLightSpaceMat(25.0f, World::lightPoints[0].pos);

			//setup viewPort size dans fbo
			setupShadowMap(cubeDepthMap);
			glCullFace(GL_FRONT);

			//pass uniforms to objectPointShadowShader
			objectPointShadowShader.use();
			objectPointShadowShader.setMat4("model", woodCubeObj.model);
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
			std::cout << "\n";

			printTimeInGame();
			printVec3("cameraPos", World::camera.pos);
			printLine(50);
			std::cout << "\n";

		};

	auto swapBuffer = [&window,&debugInfo]()
		{
			glfwSwapBuffers(window.windowPtr);
			glfwPollEvents();

			//increment time variables and debugInfo
			if (glfwGetTime() >= Time::sec + 1)
			{
				++Time::sec;
				updateTimeInGame();
				Time::currentFrame = 1;
				Time::fps = static_cast<float>(Time::totalFrame) / static_cast<float>(Time::sec);
				debugInfo();
			}
			Time::deltaSum += Time::deltaTime;
			++Time::currentFrame;
			++Time::totalFrame;
		};


	/*DES TRUCS POUR LA SPHERE*/
	glm::mat4 modele{ glm::mat4(1.0f) }; 
	//modele = modele * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, meterToWorldUnit(40.0f), 0.0f));
	//modele = modele * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f) );
	sphereShader.setMat4("model", modele);

	Texture sphereDiffuse{ loadTexture(".\\rsc\\skyDome\\sky.jpg",TextureMap::diffuse) };
	Texture cloudDiffuse{ loadTexture(".\\rsc\\skyDome\\cloud2.png",TextureMap::diffuse) };

	//Texture sphereEmission{ loadTexture(".\\rsc\\sun\\emissionMap.png",TextureMap::emission) };

	
	auto drawIcosphere = [&skyboxShader, &modele, &passThroughShader, &icosahedron, &skyBox,&sphereShader,&sphereDiffuse/*&sphereEmission*/]()
		{
			updateViewProject();
			sphereShader.use();

			glm::mat4 sphereTranslation{ glm::translate(World::sunObj.localOrigin,glm::vec3(60 * World::sunPos.x,60 * World::sunPos.y, 60 * World::sunPos.z)) };
			sphereShader.setMat4("sphereTranslation", sphereTranslation);
			sphereShader.setMat4("model", modele);
			sphereShader.setFloat("radius", 1.0f);


			glActiveTexture(GL_TEXTURE0);
			sphereShader.setInt("material.texture_diffuse1", 0);
			glBindTexture(GL_TEXTURE_2D, sphereDiffuse.ID);

			//glActiveTexture(GL_TEXTURE1);
			//sphereShader.setInt("material.texture_emission1", 1);
			//glBindTexture(GL_TEXTURE_2D, sphereEmission.ID);


			icosahedron.draw();
			//skyBox.draw(skyboxShader);		
		}; 

	/*DES TRUCS POUR LE DOME*/

	glm::mat4 modelee{ glm::mat4(1.0f) };
	modelee = modelee * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.6f, 0.0f));

	hemisphereShader.use();
	hemisphereShader.setMat4("model", modelee);
	auto drawSkyDome = [&skyFbo,&hemisphereShader,&sphereDiffuse,&hemisphere,&cloudDiffuse]()
		{
			hemisphereShader.use();

			glActiveTexture(GL_TEXTURE0);
			hemisphereShader.setInt("skybox", 0);
			glBindTexture(GL_TEXTURE_2D, sphereDiffuse.ID);

			glActiveTexture(GL_TEXTURE1);
			hemisphereShader.setInt("cloud", 1);
			glBindTexture(GL_TEXTURE_2D, cloudDiffuse.ID);

			hemisphereShader.setFloat("elaspedTime", Time::totalMinInGame);
			hemisphereShader.setInt("currentPhase", Time::dayPhase);
			hemisphereShader.setFloat("currentPhaseNextPhaseDist", Time::currentPhaseNextPhaseDist);
			hemisphereShader.setFloat("currentHourCurrentPhaseBaseHourDist", Time::currentHourCurrentPhaseBaseHourDist);

			hemisphere.draw(hemisphereShader);
		};

	/*DES TRUCS POUR LE NUAGE*/
	glm::mat4 cloudModel{ glm::mat4(1.0f) };
	//cloudModel = cloudModel * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.5f));
	cloudModel = cloudModel * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	auto drawCloud = [&cloudModel,&cloudQuad,&cloudShader,&skyFbo]()
		{

			glm::mat4 view{ glm::mat3(World::view) }; //to disable translation 
			//glm::mat4 view{ World::view }; //to disable translation 

			cloudShader.use();
			cloudShader.setMat4("model", cloudModel);
			cloudShader.setMat4("viewMat", view);

			glActiveTexture(GL_TEXTURE1);
			cloudShader.setInt("skyTexture", 1);
			glBindTexture(GL_TEXTURE_2D, skyFbo.texId);

			glDepthFunc(GL_LEQUAL); 
			cloudQuad.drawInstanced(cloudShader, "cloudTexture",4);
			glDepthFunc(GL_LESS);
		};

	//setEffect(postProcessShader, edgeDetection);

	//wireframe On
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//openDebugFile();

	
	//TUI renderloop
	//std::thread cliThread (displayTuiWindow);


	
	glfwSetTime(0);
	// 3D rendering render loop
	

	//Texture skybox { loadTexture(".\\rsc\\skyDome\\diffuseMap.jpg",TextureMap::diffuse) };

	while (!glfwWindowShouldClose(window.windowPtr))
	{	
		newFrame();
		updateViewProject();
		calcDirectLightAttrib();

		drawTerrain();

		drawIcosphere();

		drawPointShadow();

		drawSkyDome();


		swapBuffer();
	}

	glfwTerminate();
 	return 0;
}

void createAndSetLightCube(Shader& shader, std::array<Object, 2>& lightCubesObject)
{
	//create and set lightCube Object

	for (int lightCubeIndex{}; lightCubeIndex < 2; ++lightCubeIndex)
	{

		glm::vec3 color{};
		if (lightCubeIndex == 0)
			 color =  rgb(4, 217, 255) ;

		if (lightCubeIndex == 1)
			 color = rgb(242, 0, 0) ;

		lightCubesObject[lightCubeIndex].setLightPoint(color,glm::vec3(0.05f, 0.05f, 0.05f),glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),1.0f,0.0014f,0.000007f);


		lightCubesObject[lightCubeIndex].setOrbit(1.0, 30.0f, 30.0f, 6);
		lightCubesObject[lightCubeIndex].isOrbiting = true;


		lightCubesObject[lightCubeIndex].addToWorldObjects();
	}

	int index{};
	for (auto& lightCube : lightCubesObject)
	{
		World::objects[lightCube.worldObjId].enableTranslation = false;
		World::objects[lightCube.worldObjId].enableRotation = false;

		World::objects[lightCube.worldObjId].set(shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(0.4f));

		World::objects[lightCube.worldObjId].rotatePlane((index + 1) * 60); //first is 60° and second 120°


		++index;
	}
}

void createAndSetWoodCube(Shader& shader,Shader& outlineShader,Object& woodCubeObj)
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

