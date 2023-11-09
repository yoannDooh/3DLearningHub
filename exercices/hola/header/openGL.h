//define all my opengl/opengl libs/glsl classes and functions
//  
#ifndef OPENGL_H
#define OPENGL_H

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
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


class Texture
{
public:
	Texture(const char* path);
	unsigned int ID{};
	unsigned char* data{};

	enum  gl_Texture{twoD,threeD};
	enum wrapMethod {repeat,mirror,clampEdge, clampBorder};
	enum filterMethod {linear, nearest};

	gl_Texture stringToEnumTexture (std::string  input);
	wrapMethod stringToEnumWrap (std::string input);
	filterMethod stringToEnumFilter (std::string  input);

};
#endif



