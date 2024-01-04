#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <string>
#include <fstream>
#include <iostream>

/*SHADER CLASS*/
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

/*LIGHT VARIABLES*/
namespace light
{
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
}

glm::vec3 rgb(float red, float blue, float green);




