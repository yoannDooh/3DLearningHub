//define all my opengl/opengl libs/glsl classes and functions
//  
#ifndef OPENGL_H
#define OPENGL_H

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "../header/stb_image.h"


class Shader
{
public:
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// the program ID
	unsigned int ID{};

	// use/activate the shader
	void use();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void set4Float(const std::string& name, float values[4]) const;
};

class Window
{
public:
	Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle);

	int didWindowFailed{};
	GLFWwindow* windowPtr;

};


class Camera
{
public:
	// Camera C++ Class Constructor
	Camera (glm::vec3 pos,glm::vec3 front, glm::vec3 up,float speed)
	{
		this->pos = pos;
		this->front = front;
		this->up = up;
		this->speed = speed;
	}

	void setCameraPos(glm::vec3 pos)
	{
		this->pos = pos;// set employees's name
	}

	void setCameraFront(glm::vec3 front)
	{
		this->front = front;// set employees's name
	}

	void setCameraUp(glm::vec3 up)
	{
		this->up = up;// set employees's name
	}


	void moveCamera(char keyPressed,float deltaTime)
	{
		switch (keyPressed)
		{
			case 'z':
				this->pos += this->speed*deltaTime * this->front;
				break;

			case 'q':
				this->pos -= glm::normalize(glm::cross(this->front, this->up))* this->speed* deltaTime;
				break;

			case 's':
				this->pos -= this->speed*deltaTime * this->front;
				break;

			case 'd':
				this->pos += glm::normalize(glm::cross(this->front, this->up)) * this->speed*deltaTime;
				break;

			case 'u': //for up
				this->pos += this->speed * deltaTime * this->up;
				break;

			case 'w': //for down
				this->pos -= this->speed * deltaTime * this->up;
				break;

		}
	}

	glm::vec3 pos{};
	glm::vec3 front{};
	glm::vec3 up{};
	float speed{};
	float yawAngle{-90.0f};
	float pitchAngle{};
};

#endif



