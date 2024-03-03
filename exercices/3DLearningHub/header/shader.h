#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <string_view> 
#include <array>


/*SHADER CLASS*/
class Shader
{
	public:
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessellationControl, const char* tessellationEvaluation);
	Shader(){}

	// the program ID
	unsigned int ID{};

	// use/activate the shader
	void use();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void set2Float(const std::string& name, glm::vec2& value) const;
	void set2Float(const std::string& name, std::array<float, 2>values) const;
	void set3Float(const std::string& name, float values[3]) const;
	void set3Float(const std::string& name, glm::vec3& value) const;
	void set4Float(const std::string& name, float values[4]) const;
	void setMat4(const std::string& name, glm::mat4& mat) const;
};


//UBO AND SSO FUNCTIONS / VARIABLES

void genUbo0();
void fillUbo0(int dataToFill); //ubo of binding point 0 is the view/projection and camera position ubo ,
							//dataToFill : 0 for mat4 view 1: mat4 projection and 2 for vec4 viewPos 

void genUbo1(); // for pointLIghts array
void fillUbo1(int lightIndex, int dataToFill); //dataToFill it's value specify the member of struct to pass, 1 color etc.. -1 indicates to pass all member to buffer

void genUbo2(); // for direct lights
void fillUbo2(int lightIndex,int dataToFill); //dataToFill it's value specify the member of struct to pass, 1 color etc.. -1 indicates to pass all member to buffer

