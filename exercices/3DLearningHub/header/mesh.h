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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,bool cubeMapPresence);
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
	bool isThereCubeMap;

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
	Cube(float cote, std::array<float, 3>& originCoord, std::vector<Texture> textures, bool isThereCubeMap);
	//generate VAO,VBO and EBO from the provided cote and originCoor
	//there is 5 attribute, in order : coord(3 floats) -> coolors coord (https://upload.wikimedia.org/wikipedia/commons/thumb/a/af/RGB_color_solid_cube.png/220px-RGB_color_solid_cube.png) (3 floats)
	//->normal vector (3 float) -> texture coord (2 float) -> the vertex number within the 8 vertices of the cube (1 float) 
	//originCoord is bottomFace topLeft vertex 

	Cube(unsigned int vbo, unsigned int ebo); //generate VAO, bind the vbo and ebo passed as argument, and attrib pointer for coord and vertex number 
	
protected:
	void setupCube();
};

class CubeMap : public Cube
{
	public: 
		CubeMap(std::vector<const char*>& texturesPath);
		Texture texture;
		std::array<float, 72> vertices;


		void draw(Shader& shader);

	private:
		void loadTexture(std::vector<const char*>& paths);
		void setupCubeMap();
};

class Square : public Mesh
{
public:
	std::array<float,20> vertices;
	std::array<unsigned int, 6> indices;
	Texture texture;


	Square() {}
	Square(float cote, std::array<float, 2>& originCoord, Texture texture); //originCoord is topLeft vertex 
	Square(float cote, std::array<float, 2>& originCoord); //originCoord is topRight vertex 
	void draw(Shader& shader, std::string textureName);

};

//function declaration
std::vector<Texture> loadTextures(std::vector<const char*> paths, std::vector<TextureMap> types);