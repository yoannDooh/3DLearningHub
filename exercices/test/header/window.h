#ifndef WINDOW_H
#define WINDOW_H

#include "../header/shader.h"
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle);

	int didWindowFailed{};
	GLFWwindow* windowPtr;

};
#endif