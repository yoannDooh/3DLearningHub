#pragma once

#include "../header/mesh.h"
#include "../header/motion.h"


//forward declaration
glm::vec3 rgb(float red, float blue, float green);

class Object;

class AABB
{
public:
	glm::vec3 minLocalSpace{};
	glm::vec3 maxLocalSpace{};

	glm::vec3 minWorldSpace{};
	glm::vec3 maxWorldSpace{};

	glm::vec3 centroidPos{};

	glm::vec3 color{ rgb(0,0,255) };

	Cube* cube{ nullptr };
	glm::mat4 matModel{ glm::mat4(1.0f) }; //should be named matModel

	void searchMinMax(const std::vector<Vertex>& vertices, bool initMinMax = true);
	void searchMinMax(const std::vector<glm::vec3>& vertices, bool initMinMax = true);
	void constructCube();
	AABB() {}
	AABB(glm::vec3 min, glm::vec3 max);

	float volume{};

};


class AabbNode
{
public:

	AABB* data{nullptr};
	AabbNode* left {nullptr};
	AabbNode* right { nullptr } ;
};

AabbNode* allocateAabbNode();
AabbNode* allocateAabbNode(glm::vec3 min, glm::vec3 max);
void desallocateAabnode(AabbNode* node);

AabbNode* allocateTreeAabbNode(int nodeNb, glm::vec3& rootNodeSize);


class AABBTREE
{
public:
	AABBTREE(){}
	AABBTREE(std::map<int,Object*>sceneObjs);

private:

	AabbNode* rootNode{ nullptr };
	void constructTree(std::vector<Object*> objXSorted, std::vector<Object*> objYSorted, std::vector<Object*> objZSorted,AabbNode* parentNode); //recursively
	float sahCost(AABB* box,std::vector<Object*> objs ); //part du principe que volume peut pas �tre dans autre volume
};

