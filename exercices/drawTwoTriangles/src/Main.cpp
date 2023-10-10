#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float vertices[]{
					0.5f,-0.5f,0.0f,	//bottom right
					-0.5f,-0.5f,0.0f, //bottom left
					0.0f,0.5f,0.0f, //top
					
					0.5f,-0.5f,0.0f,	//bottom right
					-0.5f,-0.5f,0.0f, //bottom left
					0.0f,0.5f,0.0f, //top
};
int main()
{

}