#define _USE_MATH_DEFINES
#include <cmath>
#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <cmath>


struct Material {
	std::array<float, 3> ambient;
	std::array<float, 3> diffuse;
	std::array<float, 3> specular;
	float shininess;
};

struct DirectLight { //Directional light 
	glm::vec3 color;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct lightPointCube {
	glm::vec3 color;
	glm::vec3 pos;


	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	//attenuation variables
	float constant;
	float linearCoef;
	float squareCoef;

	glm::mat4 model{};
	glm::mat4 ellipticOrbit{};
	glm::mat4 shiftedLocalOrigin{};

};

struct SpotLight
{
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec3 direction;
	float innercutOff; //cos(angle) of inner cone angle
	float outercutOff; //cos(angle) outer cone angle 

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct Light {
	std::array<float, 3> ambient;
	std::array<float, 3> diffuse;
	std::array<float, 3> specular;
};

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

namespace Mouse
{
	float sensitivity = 0.1f;
	float lastX{ 400 };
	float lastY{ 300 };
	bool firstMouseInput{ true };
	float fov{ 45 };
}

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3 rgb(float red, float blue, float green);



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f);

int main()
{
	glm::vec3 sunLightColor{ rgb(252, 150, 1) };

	DirectLight sunLight{
		sunLightColor,
		{-0.2f, -1.0f, -0.3f},
		{ 0.2f, 0.2f, 0.2f,	},
		{ 0.5f, 0.5f, 0.5f,	},
		{ 1.0f,	 1.0f,  1.0f },
	};

	float vertices[288]{};
	unsigned int indices[36]{};
	float cubeEdge{ 1.0f };
	std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 
	std::array<Point, 8> points{};


	constructCube(vertices, indices, cubeEdge, cubeOriginCoord, points);

	glfwWindowHint(GLFW_SAMPLES, 4);
	Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
	const char* vertexPath{ ".\\shader\\vertexShader.glsl" };
	const char* fragmentPath{ ".\\shader\\fragmentShader.glsl" };

	const char* lightVertexPath{ ".\\shader\\lightSourceVertexShader.glsl" };
	const char* lightFragmentPath{ ".\\shader\\lightSourceFragmentShader.glsl" };

	Shader shader(vertexPath, fragmentPath);
	Shader lightSourceShader(lightVertexPath, lightFragmentPath);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
	glfwSetScrollCallback(window.windowPtr, scroll_callback);

	//VAO
	unsigned int VAO{};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//VBO
	unsigned int VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//EBO
	unsigned int EBO{};
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//coord attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normalVec attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Texture attribute 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//texture
	stbi_set_flip_vertically_on_load(true);

	
	Texture woodTexture (0, "woodContainer", ".\\rsc\\woodContainer.png");
	Texture specularMap(1, "specularMap", ".\\rsc\\specularMap.png");
	Texture emissionMap(2 , "emissionMap", ".\\rsc\\emissionMap.png");

	//lightVAO
	unsigned int lightVAO{};
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//coord attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//point on cube attribute
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(lightVAO);


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

	/* 
	glm::mat4 shiftedlightSourcelocalOrigin{
		glm::mat4(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos(2*M_PI / 3),sin(2*M_PI / 3),0.0f,
		0.0f,-sin(2*M_PI / 3),cos(2*M_PI / 3),0.0f,
		0.0f,0.0f,0.0f,1.0f) * lightSourcelocalOrigin
	};
	*/

	//lightCubes 
	std::array<lightPointCube, 2> lightPoints{};
	//glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	//lightPos = glm::vec3(shiftedlightSourcelocalOrigin * glm::vec4(lightPos, 0.0f) );
	glm::mat4 ellipticOrbit{ glm::mat4(1.0f) };

	for (int index{}; index < lightPoints.size(); ++index)
	{

		if (index) //index==1
			lightPoints[index].shiftedLocalOrigin =  firstLightSourceRotMatrix* lightSourcelocalOrigin;

		else
			lightPoints[index].shiftedLocalOrigin = secondLightSourceRotMatrix * lightSourcelocalOrigin ;

		lightPoints[index].pos = glm::vec3(0.0f, 0.0f, 0.0f);
		lightPoints[index].pos = glm::vec3(lightPoints[index].shiftedLocalOrigin * glm::vec4(lightPoints[index].pos, 0.0f));
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

	// --CUBE SHADER-- 
	shader.use();
	shader.setInt("material.diffuse", 0);
	shader.setInt("material.specular", 1);
	shader.setInt("material.emission", 2);
	shader.setFloat("cubeEdge", cubeEdge);
	glUniform3f(glGetUniformLocation(shader.ID, "viewPos"), camera.pos.x, camera.pos.y, camera.pos.z);

	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].pos"), lightPoints[0].pos.x, lightPoints[0].pos.y, lightPoints[0].pos.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].pos"), lightPoints[1].pos.x, lightPoints[1].pos.y, lightPoints[1].pos.z);

	//material properties 
	shader.setFloat("material.shininess", 64.0f);


	//light properties 
	glUniform3f(glGetUniformLocation(shader.ID, "sunLight.color"), sunLight.color.x, sunLight.color.y, sunLight.color.z);
	glUniform3f(glGetUniformLocation(shader.ID, "sunLight.direction"), sunLight.direction.x, sunLight.direction.x, sunLight.direction.x);

	glUniform3f(glGetUniformLocation(shader.ID, "sunLight.ambient"), sunLight.ambient.x, sunLight.ambient.y, sunLight.ambient.z);
	glUniform3f(glGetUniformLocation(shader.ID, "sunLight.diffuse"), sunLight.diffuse.x, sunLight.diffuse.y, sunLight.diffuse.z);
	glUniform3f(glGetUniformLocation(shader.ID, "sunLight.specular"), sunLight.specular.x, sunLight.specular.y, sunLight.specular.z);

	
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].ambient"), lightPoints[0].ambient.x, lightPoints[0].ambient.y, lightPoints[0].ambient.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].diffuse"), lightPoints[0].diffuse.x, lightPoints[0].diffuse.y, lightPoints[0].diffuse.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].specular"), lightPoints[0].specular.x, lightPoints[0].specular.y, lightPoints[0].specular.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].color"), lightPoints[0].color.x, lightPoints[0].color.y, lightPoints[0].color.z);


	shader.setFloat("pointLights[0].constant", lightPoints[0].constant);
	shader.setFloat("pointLights[0].linearCoef", lightPoints[0].linearCoef);
	shader.setFloat("pointLights[0].squareCoef", lightPoints[0].squareCoef);

	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].ambient"), lightPoints[1].ambient.x, lightPoints[1].ambient.y, lightPoints[1].ambient.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].diffuse"), lightPoints[1].diffuse.x, lightPoints[1].diffuse.y, lightPoints[1].diffuse.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].specular"), lightPoints[1].specular.x, lightPoints[1].specular.y, lightPoints[1].specular.z);
	glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].color"), lightPoints[1].color.x, lightPoints[1].color.y, lightPoints[1].color.z);


	shader.setFloat("pointLights[1].constant", lightPoints[1].constant);
	shader.setFloat("pointLights[1].linearCoef", lightPoints[1].linearCoef);
	shader.setFloat("pointLights[1].squareCoef", lightPoints[1].squareCoef);

	//model
	glm::mat4 model{ glm::mat4(1.0f) };
	model = glm::translate(localOrigin, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//view
	glm::mat4 view{ glm::mat4(1.0f) };
	view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//projection
	glm::mat4 projection{ glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f) };
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(shader.ID, "objectColor"), 206.0f / 255.0f, 206.0f / 255.0f, 206.0f / 255.0f);
	glUniform3f(glGetUniformLocation(shader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);


	// --LIGHTSOURCE SHADER -- //
	lightSourceShader.use();
	lightSourceShader.setFloat("cubeEdge", cubeEdge);

	//models
	glm::mat4 lightSourceModel{ glm::mat4(1.0f) };
	lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightSourceModel));

	for (int index{}; index < lightPoints.size(); ++index)
	{
		lightPoints[index].model = lightSourceModel;
	}

	//view
	glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//projection
	glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	auto drawCube = [&woodTexture,&specularMap,&emissionMap](unsigned int VAO, Shader shader)
		{
			//background color
			glm::vec3 clearColor{ rgb(29, 16, 10) };
			glClearColor(clearColor.x, clearColor.y, clearColor.z,0.0f);
			glEnable(GL_MULTISAMPLE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, woodTexture.ID);

			// bind specular map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap.ID);

			// bind emmision map
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emissionMap.ID);


			glBindVertexArray(VAO);		 
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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
		processInput(window.windowPtr);
		glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

		//ellipticOrbit = glm::translate(shiftedlightSourcelocalOrigin, glm::vec3(xElipse, 0.0f, yElipse));


		view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
		projection = glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f);

		//--CUBE-- 
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//pointLights pos

		for (int index{}; index < lightPoints.size(); ++index)
		{

			glm::vec3 merde{ glm::vec4(lightPoints[index].ellipticOrbit * glm::vec4(lightPoints[index].pos, 1.0f)) };

			lightPoints[index].pos.x = xElipse;

			if (index==0) 
			{
				glUniform3f(glGetUniformLocation(shader.ID, "pointLights[0].pos"), merde.x, merde.y, merde.z);
			}
				

			else
			{
				glUniform3f(glGetUniformLocation(shader.ID, "pointLights[1].pos"), merde.x, merde.y, merde.z);
			}
		}

		glUniform3f(glGetUniformLocation(shader.ID, "viewPos"), camera.pos.x, camera.pos.y, camera.pos.z);
		shader.setFloat("emmissionStrength", currentFrameGlow);
		drawCube(VAO, shader);


		//--LIGHTSOURCE--
		lightSourceShader.use();

		for (auto& const lightPoint : lightPoints)
		{
			glUniform3f(glGetUniformLocation(lightSourceShader.ID, "lightColor"), lightPoint.color.x, lightPoint.color.y, lightPoint.color.z );
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightPoint.model));
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr(lightPoint.ellipticOrbit));
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glBindVertexArray(lightVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		swapBuffer();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveCamera('z', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.moveCamera('q', Time::deltaTime);


	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveCamera('s', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.moveCamera('d', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.moveCamera('u', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.moveCamera('w', Time::deltaTime);

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	double xOffset{ xPos - Mouse::lastX };
	double yOffset{ Mouse::lastY - yPos };// reversed: y ranges bottom to top


	if (Mouse::firstMouseInput)
	{
		Mouse::lastX = xPos;
		Mouse::lastY = yPos;
		Mouse::firstMouseInput = false;
	}

	Mouse::lastX = xPos;
	Mouse::lastY = yPos;

	xOffset *= Mouse::sensitivity;
	yOffset *= Mouse::sensitivity;

	camera.yawAngle += xOffset;
	camera.pitchAngle += yOffset;

	if (camera.pitchAngle > 89.0f)
		camera.pitchAngle = 89.0f;

	if (camera.pitchAngle < -89.0f)
		camera.pitchAngle = -89.0f;

	glm::vec3 direction{};

	direction.x = cos(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
	direction.y = sin(glm::radians(camera.pitchAngle));
	direction.z = sin(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
	camera.front = glm::normalize(direction);


}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Mouse::fov -= (float)yoffset;

	if (Mouse::fov < 1.0f)
		Mouse::fov = 1.0f;

	if (Mouse::fov > 45.0f)
		Mouse::fov = 45.0f;
}

glm::vec3 rgb(float red, float blue, float green)
{
	return glm::vec3(red/255.0f,blue/255.0f,green/255.0f);
}