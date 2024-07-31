#pragma once

#include <string>
#include <array>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../header/shader.h"


enum TextureMap
{
	diffuse,
	specular,
	emission,
	normal,
	roughness,
	refraction,
	displacement,
	heightmap,
	shadowMap,
	cubeMap,
	shadowCubeMap,
};

enum Direction
{
	north,
	est,
	south,
	west,
	northeast,
	southeast,
	southwest,
	northwest,
};

struct Texture
{
	std::string path{};
	TextureMap type{};
	unsigned int ID{};
};

struct Vertex
{
	std::array<float, 3> coord{};     //vertices coord, in order : xyz
	std::array<float, 3> coolors{};   //color of the vertex
	std::array<float, 3> normal{};    //normal vector to the plane of the vertice 
	std::array<float, 2> textCoord{};
	float vertexNb{}; // vertex 1 2 3 4 5 6 7 or 8 on a cube 
};

//terrain Vertex struct 
struct terrainVertex
{
	std::array<float, 3> coord{};     //vertices coord, in order : xyz
	std::array<float, 2> textCoord{};
};

class Mesh
{
public:
	glm::vec3 tangent{};
	glm::vec3 bitangent{};
	std::vector<Vertex> vertices{};
	std::vector<Texture> textures{};
	std::vector<unsigned int> indices{};
	bool activateCubeMap{}; //for refraction/reflection
	bool activateShadow{};
	Mesh() {}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, bool cubeMapPresence);


	void addTexture(Texture texture);
	void draw(Shader& shader);

	unsigned int getVao();
	unsigned int getVbo();
	unsigned int getEbo();

	void setVao(unsigned int vao);
	void setVbo(unsigned int vbo);
	void setEbo(unsigned int ebo);
	void calcTB();

	

protected:
	// render data
	unsigned int VAO{}, VBO{}, EBO {};

private: 
	void setupMesh();
};

class AssimpModel //MAJORITY OF THE CODE OF THIS CLASS COME FROM LEARNOPENGL
{
public : 
	std::vector<Mesh> meshes;
	std::string path;
	std::string directoryName;
	
	AssimpModel(std::string path);

	void draw(Shader& shader);

private :
	void loadModel();
	void processNode(aiNode* node,const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterialTextures(std::vector<Texture>& textures,aiMaterial* mat, aiTextureType type,TextureMap textureType);
	bool isTextureAlreadyLoad(const char* path,int length);
	std::string direname(std::string& path); //cree un header file ou tu mets des fonctions "diverses et utiles" comme elle 
};

class Cube : public Mesh
{
public:
	//std::array<float,288> vertices; //a suprr a la place fait une alloc du vector herite par mesh
	//std::array<unsigned int,36> indices; //a suppr, meme chose à faire ici 

	Cube() {}
	Cube(float cote, std::array<float, 3>);
	Cube(std::vector<Texture> textures, float cote = 2.0f, std::array<float, 3> centerCoord = { 0.0f,0.0f,0.0f });
	//generate VAO,VBO and EBO from the provided cote and originCoor
	//there is 5 attribute, in order : coord(3 floats) -> coolors coord (https://upload.wikimedia.org/wikipedia/commons/thumb/a/af/RGB_color_solid_cube.png/220px-RGB_color_solid_cube.png) (3 floats)
	//->normal vector (3 float) -> texture coord (2 float) -> the vertex number within the 8 vertices of the cube (1 float) 
	//originCoord is bottomFace topLeft vertex 

	Cube(unsigned int vbo, unsigned int ebo, unsigned int indiceNb); //generate VAO, bind the vbo and ebo passed as argument, and attrib pointer for coord and vertex number 

protected:
	void setupCube();

};

class CubeMap :Cube
{
public:
	Texture texture{ "",cubeMap };

	CubeMap() {}
	CubeMap(std::vector<const char*>& texturesPath);

	void draw(Shader& shader);

protected:
	void loadTexture(std::vector<const char*>& paths);
	void setupCubeMap();
};

class Square : public Mesh
{
public:
	std::array<float,24> vertices;
	std::array<unsigned int, 6> indices;
	Texture texture;


	Square() {}
	Square(float cote, std::array<float, 2>& originCoord, Texture texture); //originCoord is topRight vertex 
	Square(float cote, std::array<float, 2>& originCoord); //originCoord is topRight vertex 
	void draw(Shader& shader, std::string textureName);
	void draw(Shader& shader, std::string textureName, unsigned int textureId);
	void drawInstanced(Shader& shader, std::string textureName, int count);

};

class QuadPoints : public Mesh
{
public:
	std::array<float, 8> points;

	QuadPoints() {};
	QuadPoints(std::array<float, 8>points);
	void draw();
};

class Points : public Mesh
{
public:
	std::vector<float> points;

	Points() {};
	Points(std::vector<float> pointCoord);
	void draw();
};

class Terrain
{
public:

	struct Area
	{
		std::vector<Texture> textures{};
		std::array<float, 2> xRange{}; //first is xPosLEft and second xPosRight in WORLD unit, to indicate where the texture expand and until where it stretch 
		std::array<float, 2> zRange{}; //same with z 
		std::array<float, 2> yRange{}; //range in height of the texture
	};

	struct Chunk //a chunk = one drawCall
	{
		Texture heightMap{"",TextureMap::heightmap};
		Texture shadowMap{ "",TextureMap::shadowMap };
		Texture cubeShadowMap{ "",TextureMap::shadowCubeMap };
		glm::mat4 model{ glm::mat4(1.0f) };
		std::array<float, 4> boardingChunkId{ -1,-1,-1 ,-1 }; //id of chunks in order: at north/est/south/west of the chunk object, init at -1 by default
		int width{};
		int height{};
		int startingXpos{}; 
		int startingZpos{};
		int id{};	
		unsigned int VAO{}, VBO{}, EBO{};
		bool drawShadow{ false };
		bool activateShadowMap{ false };
		bool activateCubeShadowMap{ false };
		std::vector<unsigned int> indices{};
		std::vector<Area> areas{};
		std::vector<terrainVertex> vertices{};
	};

	std::vector<Chunk> chunks{}; //chunk0 is at the center, it's the first chunk constructed with the constructor of Terrain
	int width{};
	int height{};

	

	Terrain() {};
	Terrain(int patchNb, const char* heightMapPath); //width and weight correspond to the height map's resolution and patchNb the number of patch along an axis 
	void addChunk(int targetChunkId, Direction direction, int patchNb, const char* heightMapPath); //targetChunkId is the id of the chunk the new chunk is placed next to, and direction indicate it's place north/est/south/west to the chunk
	void addArea(int chunkId, std::vector<Texture> textures, std::array<float, 2> xRange, std::array<float, 2> zRange, std::array<float, 2> yRange);
	void addShadowMap(int chunkId, Texture shadowMap);
	void addCubeShadowMap(int chunkId, Texture cubeShadowMap);

	void draw(Shader& shader);
	void drawChunk(int chunkId, Shader& shader);

private:
	void loadHeightMap(Chunk& chunk, const char* heightMapPath);
	void setupChunk(int chunkId);

};

class Icosahedron 
{
public:

	Icosahedron() {}
	Icosahedron(float radius,glm::vec3 orginCoord);
	void draw();

private:
	std::array<glm::vec3, 12> vertices;
	std::vector<unsigned int> indices{};

	unsigned int VAO{}, VBO{}, EBO{};
	void setupIcosahedron();
};

class CubeForHemisphere 
{
public :

	CubeForHemisphere();
	void draw(Shader& shader);

private: 
	std::array <std::array<float,3>, 8> vertices; //only attribute is coord
	std::array<unsigned int, 30> indices;
	unsigned int VAO{}, VBO{}, EBO{};

	void pushIndices(int startingIndexToFill,std::array<unsigned int, 3> indicesToPush);
	void setupCubeForHemisphere();
};

//function declaration
Texture loadTexture(const char* path, TextureMap type);
std::vector<Texture> loadTextures(std::vector<const char*> paths, std::vector<TextureMap> types);