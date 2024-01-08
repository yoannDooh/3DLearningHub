#include "../header/mesh.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstddef>
#include <iostream>
#include "../header/stb_image.h"

/*--MESH CLASS--*/
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	setupMesh();
}

void Mesh::draw(Shader& shader)
{
	std::array<unsigned int, 6> texturesCount{ }; //how many of each type texture there is, the count for each texture is in the same order they are defined in the 
	std::fill_n(texturesCount.begin(), 6, 1);

	for (unsigned int index = 0; index < textures.size(); index++)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		std::string number;
		std::string name;
		TextureMap type = textures[index].type;

		switch (type)
		{
		case diffuse:
			name = "diffuse";
			number = std::to_string(texturesCount[diffuse]++);
			break;

		case specular:
			name = "specular";
			number = std::to_string(texturesCount[specular]++);
			break;

		case emission:
			name = "emission";
			number = std::to_string(texturesCount[emission]++);
			break;

		case normal:
			name = "normal";
			number = std::to_string(texturesCount[normal]++);
			break;

		case roughness:
			name = "roughness";
			number = std::to_string(texturesCount[roughness]++);
			break;

		case refraction:
			name = "refraction";
			number = std::to_string(texturesCount[refraction]++);
			break;
		}

		name = ("material." + name + number);
		shader.setFloat(name.c_str(), index);

		glBindTexture(GL_TEXTURE_2D, textures[index].ID);
	}

	glActiveTexture(GL_TEXTURE0);

	 // draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void Mesh::setupMesh()
{
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//coord attribute   
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// normal attribute 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//texture coord attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textCoord));
	glBindVertexArray(0);

}

unsigned int Mesh::getVbo()
{
	return VBO;
}

unsigned int Mesh::getEbo()
{
	return EBO;
}

void Mesh::setVao(unsigned int vao)
{
	VAO = vao;
}

void Mesh::setVbo(unsigned int vbo)
{
	VBO = vbo;
}

void Mesh::setEbo(unsigned int ebo)
{
	EBO = ebo;
}

/*--CUBE CLASS--*/
Cube::Cube(float cote, std::array<float, 3>& originCoord, std::vector<Texture> textures)
{
	//vertice and face order :
	//topLeft -> bottomLeft -> bottomRight -> topRight
	//bottomFace ->  topFace ->frontFace -> backFace -> leftFace -> Rightface 
	std::array<Vertex, 8> cubeVertices{};

	int axisIndex{};
	for (float axisValue : originCoord)
	{
		cubeVertices[0].coord[axisIndex] = axisValue;
		++axisIndex;
	}

	for (int faceIndex{ 1 }; faceIndex <= 2; ++faceIndex)
	{
		if (faceIndex == 1)
		{
			for (int cubeVerticesIndex{ 1 }; cubeVerticesIndex <= 4; ++cubeVerticesIndex)
			{
				switch (cubeVerticesIndex)
				{
				case 1: //bottomLeft
					cubeVertices[1].coord[0] = cubeVertices[cubeVerticesIndex - 1].coord[0];
					cubeVertices[1].coord[1] = cubeVertices[cubeVerticesIndex - 1].coord[1];
					cubeVertices[1].coord[2] = cote + cubeVertices[cubeVerticesIndex - 1].coord[2];
					break;

				case 2: //bottomRight
					cubeVertices[2].coord[0] = cote + cubeVertices[cubeVerticesIndex - 1].coord[0];
					cubeVertices[2].coord[1] = cubeVertices[cubeVerticesIndex - 1].coord[1];
					cubeVertices[2].coord[2] = cubeVertices[cubeVerticesIndex - 1].coord[2];
					break;

				case 3: //topRight
					cubeVertices[3].coord[0] = cubeVertices[cubeVerticesIndex - 1].coord[0];
					cubeVertices[3].coord[1] = cubeVertices[cubeVerticesIndex - 1].coord[1];
					cubeVertices[3].coord[2] = cubeVertices[cubeVerticesIndex - 1].coord[2] - cote;
					break;
				}
			}
			continue;
		}

		for (int cubeVerticesIndex{ 0 }; cubeVerticesIndex < 4; ++cubeVerticesIndex)
		{
			switch (cubeVerticesIndex)
			{

			case 0: //topLeft
				cubeVertices[4].coord[0] = cubeVertices[0].coord[0];
				cubeVertices[4].coord[1] = cote + cubeVertices[0].coord[1];
				cubeVertices[4].coord[2] = cubeVertices[0].coord[2];
				break;

			case 1: //bottomLeft
				cubeVertices[5].coord[0] = cubeVertices[1].coord[0];
				cubeVertices[5].coord[1] = cote + cubeVertices[1].coord[1];
				cubeVertices[5].coord[2] = cubeVertices[1].coord[2];
				break;

			case 2: //bottomRight
				cubeVertices[6].coord[0] = cubeVertices[2].coord[0];
				cubeVertices[6].coord[1] = cote + cubeVertices[2].coord[1];
				cubeVertices[6].coord[2] = cubeVertices[2].coord[2];
				break;

			case 3: //topRight
				cubeVertices[7].coord[0] = cubeVertices[3].coord[0];
				cubeVertices[7].coord[1] = cote + cubeVertices[3].coord[1];
				cubeVertices[7].coord[2] = cubeVertices[3].coord[2];
				break;
			}
		}
	}

	int cubeVerticesIndex{ 0 };
	for (Vertex& vertex : cubeVertices)
	{
		int coolorIndex{};
		for (int coordIndex{ 2 }; coordIndex >= 0; --coordIndex)
		{
			if (vertex.coord[coordIndex] == cubeVertices[0].coord[coordIndex])
				vertex.coolors[coolorIndex] = 0.0f;

			else
				vertex.coolors[coolorIndex] = 1.0f;

			++coolorIndex;
		}
		++cubeVerticesIndex;
	}

	cubeVerticesIndex = 0;

	int verticeIndex{};
	auto assignValue = [this, &cubeVertices, &verticeIndex](int cubeVerticesIndex, int faceVertex, int faceNr)
		{
			//facePoint point on the face is : 0=topLeft; 1=bottomLeft; 2=bottomRight 3=topRight


			// assign coord
			if (verticeIndex == 0)
				vertices[verticeIndex] = cubeVertices[cubeVerticesIndex].coord[0];
			else
				vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coord[0];

			vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coord[1];
			vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coord[2];

			// assign coolors coord
			vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coolors[0];
			vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coolors[1];
			vertices[++verticeIndex] = cubeVertices[cubeVerticesIndex].coolors[2];

			//assign normal coord
			switch (faceNr)
			{
			case 0: //bottomFace
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = -1.0f;
				vertices[++verticeIndex] = 0.0f;
				break;

			case 1: //topFace 
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 1.0f;
				vertices[++verticeIndex] = 0.0f;
				break;

			case 2: //frontface
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 1.0f;
				break;

			case 3: //backface
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = -1.0f;
				break;

			case 4: //leftface
				vertices[++verticeIndex] = -1.0f;
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 0.0f;
				break;

			case 5: //rightface
				vertices[++verticeIndex] = 1.0f;
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 0.0f;
				break;
			}

			//assign texture coord
			switch (faceVertex)
			{
			case 0:
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 1.0f;
				break;

			case 1:
				vertices[++verticeIndex] = 0.0f;
				vertices[++verticeIndex] = 0.0f;
				break;

			case 2:
				vertices[++verticeIndex] = 1.0f;
				vertices[++verticeIndex] = 0.0f;
				break;

			case 3:
				vertices[++verticeIndex] = 1.0f;
				vertices[++verticeIndex] = 1.0f;
				break;

			}


			//assign cubeVertex number 
			vertices[++verticeIndex] = cubeVerticesIndex;
		};

	while (verticeIndex < 192)
	{
		for (int faceindex{}; faceindex < 6; ++faceindex)
		{
			switch (faceindex)
			{
			case 0: //bottomFace
				for (int pointIndex{}; pointIndex < 4; ++pointIndex)
				{
					assignValue(pointIndex, pointIndex, 0);
				}
				break;

			case 1: //topFace
				for (int pointIndex{ 4 }; pointIndex < 8; ++pointIndex)
				{
					assignValue(pointIndex, pointIndex - 4, 1);
				}
				break;

			case 2: //frontFace
				assignValue(5, 0, 2); //topFace bottomLeft
				assignValue(1, 1, 2); //bottomFace bottomLeft
				assignValue(2, 2, 2); //bottomFace bottomRight
				assignValue(6, 3, 2); //topFace bottomRight
				break;

			case 3: //backFace
				assignValue(4, 0, 3); //topFace topLeft
				assignValue(0, 1, 3); //bottomFace topLeft
				assignValue(3, 2, 3); //bottomFace topRight
				assignValue(7, 3, 3); //topFace topRight
				break;

			case 4: //leftFace
				assignValue(4, 0, 4); //topFace topLeft
				assignValue(0, 1, 4); //bottomFace topLeft
				assignValue(1, 2, 4); //bottomFace bottomLeft
				assignValue(5, 3, 4); //topFace bottomLeft
				break;

			case 5: //rightFace
				assignValue(7, 0, 5); //topFace topRight
				assignValue(3, 1, 5); //bottomFace topRight
				assignValue(2, 2, 5); //bottomFace bottomRight
				assignValue(6, 3, 5); //topFace bottomRight
				break;
			}
		}
	}

	int indicesIndex{};
	int currentFace{ 0 };
	while (indicesIndex < 36)
	{
		for (int count{}; count < 3; ++count) //first triangle indices
		{
			if (count == 2)
				indices[indicesIndex] = (currentFace * 4) + count + 1;
			else
				indices[indicesIndex] = (currentFace * 4) + count;

			++indicesIndex;
		}

		for (int count{ (currentFace * 4) + 1 }; count <= currentFace * 4 + 3; ++count) //first triangle indices
		{
			indices[indicesIndex] = count;
			++indicesIndex;
		}

		if (indicesIndex % 6 == 0)
		{
			++currentFace;
		}
	}

	//Assign texturesMap
	for (const auto& texture : textures)
	{
		this->textures.push_back(texture);
	}

	//create VAO,EBO,VBO
	setupCube();
}

void Cube::constructLightCube(unsigned int Vbo,unsigned int Ebo)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Ebo);

	//coord attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//point on cube attribute
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Cube::draw(Shader& shader)
{
	std::array<unsigned int, 6> texturesCount{ }; //how many of each type texture there is, the count for each texture is in the same order they are defined in the 
	std::fill_n(texturesCount.begin(), 6, 1);

	for (unsigned int index = 0; index < textures.size(); index++)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		std::string number;
		std::string name;
		TextureMap type = textures[index].type;

		switch (type)
		{
		case diffuse:
			name = "texture_diffuse";
			number = std::to_string(texturesCount[diffuse]++);
			break;

		case specular:
			name = "texture_specular";
			number = std::to_string(texturesCount[specular]++);
			break;

		case emission:
			name = "texture_emission";
			number = std::to_string(texturesCount[emission]++);
			break;

		case normal:
			name = "texture_normal";
			number = std::to_string(texturesCount[normal]++);
			break;

		case roughness:
			name = "texture_roughness";
			number = std::to_string(texturesCount[roughness]++);
			break;

		case refraction:
			name = "texture_refraction";
			number = std::to_string(texturesCount[refraction]++);
			break;
		}

		name = ("material." + name + number);
		shader.setInt(name.c_str(), index);

		glBindTexture(GL_TEXTURE_2D, textures[index].ID);
	}

	//glActiveTexture(GL_TEXTURE0); 

	 // draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Cube::setupCube()
{
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	//coord attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);

	// normal attribute 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float) ) );

	//texture coord attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float) ) );

	//vertex number attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float) ) );

	glBindVertexArray(0);
}

/*--Texture FUNCTION--*/
std::vector<Texture> loadTextures(std::vector<const char*> pathes, std::vector<TextureMap> types)
{
	std::vector<Texture> textures{};
	int width{}, height{}, nrChannels{};
	unsigned char* data{};

	int index{};
	for (auto& const path : pathes)
	{
		Texture texture;

		unsigned int textureId{};
		glGenTextures(1, &textureId);
		texture.ID = textureId;
		texture.type = types[index];

		textures.push_back(texture);

		data = stbi_load(path, &width, &height, &nrChannels, 0);

		if (data)
		{
			GLenum format = GL_RGBA;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, texture.ID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}

		else
			std::cout << "Failed to load : texture" << texture.type << std::endl;

		++index;
	}

	return textures;
}

Texture loadCubemap(std::vector<const char*> pathes)
{
	Texture texture{};
	int width{}, height{}, nrChannels{};
	unsigned char* data{};
	int index{};

	unsigned int textureId{};
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	texture.ID = textureId;

	for (auto& const path : pathes)
	{
		data = stbi_load(path, &width, &height, &nrChannels, 0);

		if (data)
		{
			GLenum format = GL_RGBA;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		else
		{
			std::cerr << "failed to load face number " << index << " of cubeMap";
		}
	
		++index;
	}


	//texture filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	return texture;
}






