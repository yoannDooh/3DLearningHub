#include <iostream>
#include <thread>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "../header/stb_image.h"
#include "../header/window.h"
#include "../header/shader.h"
#include "../header/motion.h"
#include "../header/mesh.h"
#include "../header/tuiWindow.h"
#include "../header/renderLoop.h"


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
	glfwWindowHint(GLFW_SAMPLES, 8);
	Window window(2,2,"3DLearningHub");

	//rendering parameters
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_CLAMP);

	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window.windowPtr, window_size_callback);

	//init Uniforms buffer 
	genUbo0();
	genUbo1();
	genUbo2();

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

	//init Framebuffers
	FrameBuffer fbo(true, true);
	FrameBuffer skyFbo(true, true);

	//frameBuffer for shadowMap
	ShadowBuffer depthMap;
	depthMap.genDepthMapBuff();
	//depthMap.genDepthMapLightSpaceMat(1000.0f, glm::vec3(-2532.0f, 7877.0f, -2065.0f), glm::vec3(0.0f));

	//frameBuffer for cubeShadowMap
	ShadowBuffer cubeDepthMap;
	cubeDepthMap.genCubeMapBuff();

	//set Time Speed 
	Time::timeAccelerator = 50.0f;

	/*MESHES INIT*/

	//CubeMap mesh for skybox [DEPRECATED]
	std::vector<const char*> skyBoxTextPaths{
			".\\rsc\\skybox\\right.jpg",
			".\\rsc\\skybox\\left.jpg",
			".\\rsc\\skybox\\top.jpg",
			".\\rsc\\skybox\\bottom.jpg",
			".\\rsc\\skybox\\front.jpg",
			".\\rsc\\skybox\\back.jpg"
	};

	CubeMap skyBox(skyBoxTextPaths);

	//Cube mesh for wood textured cube
	float cubeEdge{ 1.0f };
	std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 

	Cube woodCube (cubeEdge, cubeOriginCoord, loadTextures({".\\rsc\\woodCube\\woodContainer.png",".\\rsc\\woodCube\\specularMap.png" ,".\\rsc\\woodCube\\emissionMap.png" }, {diffuse,specular,emission}) );
	woodCube.activateCubeMap = true;
	woodCube.activateShadow = true;
	woodCube.addTexture(skyBox.texture);
	
	//Cube mesh for light moving Cubes
	Cube lightCube( woodCube.getVbo(), woodCube.getEbo(),36 );

	//Terrain mesh
	Terrain terrain(2,".\\rsc\\terrain\\heightMaps\\drole.png");
	//terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\brickWall\\diffuseMap.jpg",".\\rsc\\terrain\\brickWall\\normalMap.jpg" }, { diffuse,normal }), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\coralStoneWall\\diffuseMap.jpg",".\\rsc\\terrain\\coralStoneWall\\normalMap.jpg",".\\rsc\\terrain\\coralStoneWall\\displacementMap.jpg" }, { diffuse,normal,displacement }), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addChunk(0, north, 2, ".\\rsc\\terrain\\heightMaps\\heightMap2.jpeg");
	//terrain.addChunk(1, west, 2, ".\\rsc\\terrain\\heightMaps\\b.jpg");
	
	//icoSphere mesh
	Icosahedron icosahedron(1.0f,glm::vec3(0.0f,0.0f,0.0f));

	//quad for geometry Shader 
	Points quadPoints(points);
	Points circle(circleCenter);

	//square mesh for postProcess
	std::array<float, 2> origin{ 1.0f,1.0f };
	Square quad(2.0, origin);

	//square mesh for clouds [DEPRECATED]
	Square cloudQuad(2.0, origin, loadTexture(".\\rsc\\skydome\\cloud2.png", diffuse));

	//hemisphere mesh
	CubeForHemisphere hemisphere;

	
	/*OBJECTS INIT*/
	std::array<Object, 2> lightCubesObject{};
	Object woodCubeObj;

	//set models	
	createAndSetWoodCube(objectShader, outlineShader, woodCubeObj);
	createAndSetLightCube(lightSourcesShader,lightCubesObject);

	/*TEXTURES*/

	//shadowMap
	Texture shadowMap;
	shadowMap.type = TextureMap::shadowMap;
	
	//cubeShadowMap
	Texture cubeShadowMap;
	cubeShadowMap.type = TextureMap::shadowCubeMap;

	Texture sphereDiffuse{ loadTexture(".\\rsc\\skyDome\\sky.jpg",TextureMap::diffuse) };

	Texture cloudDiffuse{ loadTexture(".\\rsc\\skyDome\\cloud2.png",TextureMap::diffuse) };

	//Texture skybox { loadTexture(".\\rsc\\skyDome\\diffuseMap.jpg",TextureMap::diffuse) };
	//Texture sphereEmission{ loadTexture(".\\rsc\\sun\\emissionMap.png",TextureMap::emission) };

	/*VARIABLES FOR LAMBDA FUNCTIONS*/

	//drawTerrain variables
	glm::mat4 idMat{ glm::mat4(1.0f) }; //identity matrix

	//drawIcosphere variables and set icosphereModel
	glm::mat4 sphereModel{ glm::mat4(1.0f) };
	//sphereModel = sphereModel * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, meterToWorldUnit(40.0f), 0.0f));
	//sphereModel = sphereModel * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f) );
	sphereShader.setMat4("model", sphereModel);

	//drawCloud variables
	glm::mat4 cloudModel{ glm::mat4(1.0f) };
	//cloudModel = cloudModel * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.5f));
	cloudModel = cloudModel * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	//set hemisphere 
	glm::mat4 hemisphereModel{ glm::mat4(1.0f) };
	hemisphereModel = hemisphereModel * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.6f, 0.0f));
	hemisphereShader.use();
	hemisphereShader.setMat4("model", hemisphereModel);

	/*LAMBDA FUNCTIONS FOR RENDER LOOP*/
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

	auto drawTerrain = [&terrainShader,&terrain,&skyBox,&skyboxShader,&idMat]()
		{
			float a{ 1.0f / static_cast<float>(terrain.width) };
			float b{ 1.0f / static_cast<float>(terrain.height) };

			terrainShader.use();
			terrainShader.setMat4("model", idMat);
			terrainShader.setFloat("maxDistLod", 300);


			terrainShader.setFloat("area1.shininess", 512.0f);
			terrainShader.setFloat("area1.specularIntensity", 0.0f);


			setLighting();
			terrain.draw(terrainShader);

			//DRAW IN LAST
			//skyBox.draw(skyboxShader);

		};

	auto drawShadow = [&idMat,&objectDirectShadowShader,&terrainDirectShadowShader,&depthMap,&shadowMap,&woodCube,&objectShader,&drawScene,&terrainShader,&drawTerrain,&terrain,&woodCubeObj]()
		{

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
			terrainDirectShadowShader.setMat4("model", idMat);
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

	auto swapBuffer = [&window]()
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
				//debugInfo();
			}
			Time::deltaSum += Time::deltaTime;
			++Time::currentFrame;
			++Time::totalFrame;
		};

	auto drawIcosphere = [&skyboxShader, &sphereModel, &passThroughShader, &icosahedron, &skyBox,&sphereShader,&sphereDiffuse/*&sphereEmission*/]()
		{
			updateViewProject();
			sphereShader.use();

			glm::mat4 sphereTranslation{ glm::translate(World::sunObj.localOrigin,glm::vec3(60 * World::sunPos.x,60 * World::sunPos.y, 60 * World::sunPos.z)) };
			sphereShader.setMat4("sphereTranslation", sphereTranslation);
			sphereShader.setMat4("model", sphereModel);
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


	/*RENDER PARAMETERS / EFFECTS*/

	//setEffect(postProcessShader, edgeDetection);

	//wireframe On
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//openDebugFile();

	//TUI renderloop
	std::thread cliThread (displayTuiWindow);
	
	glfwSetTime(0);
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


