#pragma once

#include <string>
#include <array>
#include <vector>
#include "../header/shaderAndLight.h"

enum TextureMap
{
	diffuse,
	specular,
	emission,
	normal,
	roughness,
	refraction,
};

struct Texture
{
	unsigned int ID;
	TextureMap type{};
	std::string path{};
};

struct Vertex
{
	std::array<float, 3> coord{};     //vertices coord, in order : xyz
	std::array<float, 3> coolors{};   //rgb cube model 
	std::array<float, 3> normal{};    //normal vector to the plane of the vertice 
	std::array<float, 2> textCoord{};
	float vertexNb{}; // vertex 1 2 3 4 5 6 7 or 8 on the cube 
};

class Mesh
{
public:
	Mesh() {}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	void draw(Shader& shader);

	unsigned int getVbo();
	unsigned int getEbo();

	void setVao(unsigned int vao);
	void setVbo(unsigned int vbo);
	void setEbo(unsigned int ebo);

	

protected:
	// render data
	unsigned int VAO{}, VBO{}, EBO {};
	
private: 

	void setupMesh();
};

class Cube : public Mesh
{
public:
	std::array<float,288> vertices;
	std::array<unsigned int,36> indices;
	void draw(Shader& shader);

	Cube() {}
	Cube(float cote, std::array<float, 3>& originCoord, std::vector<Texture> textures);
	void constructLightCube(unsigned int vbo, unsigned int ebo); //construct lightCube VAO/VBO/EBO 
		
	void setupCube();
};


std::vector<Texture> loadTextures(std::vector<const char*> pathes, std::vector<TextureMap> types);

struct Point
{
	std::array<float, 3> coord;     //vertices coord, in order : xyz
	std::array<float, 3> coolors;   //vertices coolors in order : rgb
	std::array<float, 3> normal;    //normal vector to the plane of the vertice 
};

void constructCube(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point);

void compare(Cube cube, float vertices[216], unsigned int indices[36]);

//in order : coord/colorCoord/normal/TextCoord/number of the point 
