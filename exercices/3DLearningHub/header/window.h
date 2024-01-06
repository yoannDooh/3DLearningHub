#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "../header/motion.h"

class Window
{
public:
	Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle);

	int didWindowFailed{};
	GLFWwindow* windowPtr;

};

/*WINDOW SETTINGS*/
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

/*INPUT LISTENER FUNCTIONS*/
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
