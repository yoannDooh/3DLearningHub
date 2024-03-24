#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "../header/motion.h"

class Window
{
public:
	Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle);
	Window(int widthRatio, int heightRatio, const char* windowTitle);

	int didWindowFailed{};
	GLFWwindow* windowPtr;

};

/*WINDOW SETTINGS*/
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

/*INPUT LISTENER FUNCTIONS*/
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);
void setWindowRatioToFullSceen(unsigned int widthRatio, unsigned int heightRatio); //1 means fullScrean, 2 means width and height divided by two...
