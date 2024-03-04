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
void printTimeInGame();
void printLine(int dashNb);
void createAndSetLightCube(Shader& shader, std::array<Object, 2>& lightCubesObject);
void createAndSetWoodCube(Shader& shader, Shader& outlineShader, Object& woodCubeObj);
void printCurrentDayPhase();

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
	Shader passThroughShader(".\\shader\\passThrough\\vertex.glsl", ".\\shader\\passThrough\\fragment.glsl");
	Shader sphereShader(".\\shader\\sphere\\vertex.glsl", ".\\shader\\sphere\\fragment.glsl", ".\\shader\\sphere\\TCS.glsl", ".\\shader\\sphere\\TES.glsl");

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

	//init Uniforms buffer 
	genUbo0();
	genUbo1();
	genUbo2();

	//callback functions/ window inputMode
	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);
	glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set models	
	createAndSetWoodCube(objectShader, outlineShader, woodCubeObj);

	createAndSetLightCube(lightSourcesShader,lightCubesObject);

	//terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\brickWall\\diffuseMap.jpg",".\\rsc\\terrain\\brickWall\\normalMap.jpg" }, { diffuse,normal }), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addArea(0, loadTextures({ ".\\rsc\\terrain\\coralStoneWall\\diffuseMap.jpg",".\\rsc\\terrain\\coralStoneWall\\normalMap.jpg",".\\rsc\\terrain\\coralStoneWall\\displacementMap.jpg" }, { diffuse,normal,displacement}), { meterToWorldUnit(-9), meterToWorldUnit(9) }, { meterToWorldUnit(-9),meterToWorldUnit(9) }, { 0.0f, 0.0f });
	terrain.addChunk(0, north, 2, ".\\rsc\\terrain\\heightMaps\\heightMap2.jpeg");
	//terrain.addChunk(1, west, 2, ".\\rsc\\terrain\\heightMaps\\b.jpg");

	FrameBuffer fbo(true, true);

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
	depthMap.genDepthMapLightSpaceMat(1000.0f, glm::vec3(-2532.0f, 7877.0f, -2065.0f), glm::vec3(0.0f));

	ShadowBuffer cubeDepthMap;
	cubeDepthMap.genCubeMapBuff();

	auto drawShadow = [&model,&objectDirectShadowShader,&terrainDirectShadowShader,&depthMap,&shadowMap,&woodCube,&objectShader,&drawScene,&terrainShader,&drawTerrain,&terrain,&woodCubeObj]()
		{

			//lightSpaceMat = toDirectionalLightSpaceMat(1000.0f,World::camera.pos, glm::vec3(0.0f) );
			
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

	glm::mat4 modele{ glm::mat4(1.0f) }; 
	modele = modele * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, meterToWorldUnit(40.0f), 0.0f));
	modele = modele * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f) );
	sphereShader.setMat4("model", modele);

	auto drawIcosphere = [&skyboxShader, &modele, &passThroughShader, &icosahedron, &skyBox,&sphereShader]()
		{
			updateViewProject();
			sphereShader.use();
			modele = modele * glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(0.0, 1.0f, 0.0f));
			glm::mat4 sphereTranslation{ calSunPos() };
			sphereShader.setMat4("sphereTranslation", sphereTranslation);
			sphereShader.setMat4("model", modele);
			sphereShader.setFloat("radius", 1.0f);
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

	/*--------------------*/
		//drawScene();
		//drawTerrain();
	/*--------------------*/

		drawIcosphere();
		calcDirectLightAttrib();

		drawPointShadow();

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

void printColor(glm::vec3 color)
{
	std::cout << '\t' << "RED  : " << color.x << '\n';
	std::cout << '\t' << "BLUE : " << color.y << '\n';
	std::cout << '\t' << "GREEN : " << color.z << '\n';
	std::cout << "\n\n";
}

void printTimeInGame()
{
	std::cout << "TIME IN GAME " << ":" << '\n';

	std::cout << '\t' << "Day : " << Time::timeInGame.day << '\n';
	std::cout << '\t' << "Hour : " << Time::timeInGame.hour << '\n';
	std::cout << '\t' << "Min : " << Time::timeInGame.min << '\n';
	std::cout << '\t' << "Sec : " << Time::timeInGame.sec << '\n';
	std::cout << '\t' << "SunAzimuth : " << World::sunAzimuth << '\n';
	std::cout << '\t' << "SunAltitude : " << World::sunAltitude << '\n';
	printCurrentDayPhase();
	std::cout << "\n\n";

	std::cout << "\n" << "couleur sunlight" << '\n';
	printColor(World::directLights[0].color);
	std::cout << "\n\n";
}	

void printCurrentDayPhase()
{
	std::cout << '\t' << "Phase of the Day : ";
	switch (Time::dayPhase)
	{
	case dawn:
		std::cout  << "Dawn";
		break;

	case daytime:
		std::cout << "daytime";
		break;

	case sunset:
		std::cout << "sunset";
		break;

	case night:
		std::cout << "night";
		break;
	
	}
	std::cout << '\n';
}

void printLine(int dashNb)
{
	for (int count{};count<= dashNb; ++count)
	{
		std::cout << '-';
	}
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
	woodCubeObj.enableRotation = false;
	woodCubeObj.isOutLined = true;
	woodCubeObj.isGlowing = true;
	woodCubeObj.materialShininess = 64.0f;
	woodCubeObj.shaderOutline = outlineShader;
	woodCubeObj.setGlow(glm::vec3{ rgb(255, 255, 0) }, 0.25, 0.9);
	woodCubeObj.set(shader, glm::vec3(0.0f, meterToWorldUnit(0.1f), 0.0f), glm::vec3(0.0f), 0.0f, glm::vec3(10.0f));
}

