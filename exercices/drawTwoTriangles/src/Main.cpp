#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
static void readGlslFile(std::string filePath);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float vertices[]{
					//first triangle (on the left)
					0.0f,-0.5f,0.0f,//bottom right
					-0.5f,-0.5f,0.0f, //bottom left
					-0.25f,0.5f,0.0f //top			
};

float vertices2[]{
				//second triangle (on the right)
					0.0f,-0.5f,0.0f,	//bottom left
					0.5f,-0.5f,0.0f, //bottom right
					0.25f,0.5f,0.0f //top
};

namespace Shader
{
	std::string shader{};
}

const char* vertexShaderSrc = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSrc1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSrc2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
"}\0";

int main()
{
	std::cout << fragmentShaderSrc1 << '\n';

	readGlslFile("C:\\Users\\yoann\\OneDrive\\Bureau\\Proggramation\\apprendre\\openGL\\exercices\\drawTwoTriangles\\src\\vertexShader.glsl");
	vertexShaderSrc = Shader::shader.c_str();
	std::cout << vertexShaderSrc << '\n';

	readGlslFile("C:\\Users\\yoann\\OneDrive\\Bureau\\Proggramation\\apprendre\\openGL\\exercices\\drawTwoTriangles\\src\\fragmentShader.glsl");
	fragmentShaderSrc1 = Shader::shader.c_str();

	std::cout << fragmentShaderSrc1;



	//init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//create window
	GLFWwindow* window{ glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"triangles",NULL,NULL) };
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//fragment shader
	unsigned int fragmentShader(glCreateShader(GL_FRAGMENT_SHADER));
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc1, NULL);
	glCompileShader(fragmentShader);

	//vertex shader
	unsigned int vertexShader(glCreateShader(GL_VERTEX_SHADER));
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);


	//debug vertex shader
	int success{};
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}


	//debug fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}

	//link shader
	unsigned int shaderProgram(glCreateProgram());
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, vertexShader);
	glLinkProgram(shaderProgram);

	//bind and debug fragment shader 2
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc2, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::FRAGMENT::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}


	unsigned int shaderProgram2(glCreateProgram());
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader);
	glLinkProgram(shaderProgram2);

	//debug shader program
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}


	//delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//initialize VAO
	unsigned int VAO{};
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	//init VBO
	unsigned int VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//link vertex to shader attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	//init VAO2
	unsigned int VAO2{};
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);

	//init VBO2
	unsigned int VBO2{};
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	//render loop 
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(1.0f, 72.0f / 255.0f, 95.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//activate shader program
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		
		glUseProgram(shaderProgram2);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

static void readGlslFile(std::string filePath)
{
	Shader::shader.erase(Shader::shader.begin(), Shader::shader.end() );
	std::string offStream{};
	std::ifstream shaderFile(filePath);

	while (std::getline(shaderFile, offStream))
	{
		if (offStream != "")
		{
			Shader::shader = Shader::shader + '\n' + offStream;
		}
	}
	Shader::shader = Shader::shader + '\0';
}

void processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0,width,height);
}
