#include "../header/collision.h"
#include <functional>

/*--AABB CLASS--*/

AABB::AABB(glm::vec3 min, glm::vec3 max)
{
	minWorldSpace = min;
	maxWorldSpace = max;

	float cote{ max.y - min.y };

	centroidPos = max - min / 2.0f;

	float scale{ cote / 2.0f }; //2 because in local space -1 to 1 
	//glm::mat3 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	//min = scaleMat * min;

	//matModel = glm::translate(glm::mat4(1.0f), glm::vec3(minWorldSpace.x - min.x, minWorldSpace.y - min.y, minWorldSpace.z - min.z));
	//matModel *= scaleMat;

	//LE PROBLEME EST LA 
}

void AABB::searchMinMax(const std::vector<Vertex>& vertices, bool initMinMax)
{
	if (initMinMax)
	{
		minLocalSpace = vertices[0].coord;
		maxLocalSpace = vertices[0].coord;

		for (int index{ 1 }; index < vertices.size(); ++index)
		{
			if (vertices[index].coord.x < minLocalSpace.x)
				minLocalSpace.x = vertices[index].coord.x;

			if (vertices[index].coord.y < minLocalSpace.y)
				minLocalSpace.y = vertices[index].coord.y;

			if (vertices[index].coord.z < minLocalSpace.z)
				minLocalSpace.z = vertices[index].coord.z;

			if (vertices[index].coord.x > maxLocalSpace.x)
				maxLocalSpace.x = vertices[index].coord.x;

			if (vertices[index].coord.y > maxLocalSpace.y)
				maxLocalSpace.y = vertices[index].coord.y;

			if (vertices[index].coord.z > maxLocalSpace.z)
				maxLocalSpace.z = vertices[index].coord.z;
		}
	}

	else
	{
		for (int index{}; index < vertices.size(); ++index)
		{
			if (vertices[index].coord.x < minLocalSpace.x)
				minLocalSpace.x = vertices[index].coord.x;

			if (vertices[index].coord.y < minLocalSpace.y)
				minLocalSpace.y = vertices[index].coord.y;

			if (vertices[index].coord.z < minLocalSpace.z)
				minLocalSpace.z = vertices[index].coord.z;

			if (vertices[index].coord.x > maxLocalSpace.x)
				maxLocalSpace.x = vertices[index].coord.x;

			if (vertices[index].coord.y > maxLocalSpace.y)
				maxLocalSpace.y = vertices[index].coord.y;

			if (vertices[index].coord.z > maxLocalSpace.z)
				maxLocalSpace.z = vertices[index].coord.z;
		}
	}
}

void AABB::searchMinMax(const std::vector<glm::vec3>& vertices, bool initMinMax)
{
	if (initMinMax)
	{
		minLocalSpace = vertices[0];
		maxLocalSpace = vertices[0];

		for (int index{ 1 }; index < vertices.size(); ++index)
		{
			if (vertices[index].x < minLocalSpace.x)
				minLocalSpace.x = vertices[index].x;

			if (vertices[index].y < minLocalSpace.y)
				minLocalSpace.y = vertices[index].y;

			if (vertices[index].z < minLocalSpace.z)
				minLocalSpace.z = vertices[index].z;

			if (vertices[index].x > maxLocalSpace.x)
				maxLocalSpace.x = vertices[index].x;

			if (vertices[index].y > maxLocalSpace.y)
				maxLocalSpace.y = vertices[index].y;

			if (vertices[index].z > maxLocalSpace.z)
				maxLocalSpace.z = vertices[index].z;
		}
	}

	else
	{
		for (int index{}; index < vertices.size(); ++index)
		{
			if (vertices[index].x < minLocalSpace.x)
				minLocalSpace.x = vertices[index].x;

			if (vertices[index].y < minLocalSpace.y)
				minLocalSpace.y = vertices[index].y;

			if (vertices[index].z < minLocalSpace.z)
				minLocalSpace.z = vertices[index].z;

			if (vertices[index].x > maxLocalSpace.x)
				maxLocalSpace.x = vertices[index].x;

			if (vertices[index].y > maxLocalSpace.y)
				maxLocalSpace.y = vertices[index].y;

			if (vertices[index].z > maxLocalSpace.z)
				maxLocalSpace.z = vertices[index].z;
		}
	}
}

void AABB::constructCube()
{
	cube = new Cube(minLocalSpace, maxLocalSpace);
}


AabbNode* allocateTreeAabbNode(int nodeNb, glm::vec3& rootNodeSize)
{
	AabbNode* tree = new AabbNode[nodeNb];

	//init root node
	tree->data = new AABB();

	return tree;
}

AabbNode* allocateAabbNode()
{
	AabbNode* node = new AabbNode();
	node->data = new AABB();

	return node;
}

AabbNode* allocateAabbNode(glm::vec3 min, glm::vec3 max)
{
	AabbNode* node = new AabbNode();
	node->data = new AABB(min, max);
	node->data->minWorldSpace = min;
	node->data->maxWorldSpace = max;

	return node;
}

void desallocateAabnode(AabbNode* node)
{
	delete(node->data);
	delete(node);
}

/*--AABB TREE--*/
AABBTREE::AABBTREE(std::map<int, Object*>sceneObjs)
{
	rootNode = new AabbNode;

	std::vector<Object*> tempSceneObjXSorted{};
	std::vector<Object*> tempSceneObjYSorted{};
	std::vector<Object*> tempSceneObjZSorted{};


	//fil temp arrays
	for (const auto& iter : sceneObjs)
	{
		if (tempSceneObjXSorted.size() == 0)
		{
			tempSceneObjXSorted.push_back(iter.second);
			tempSceneObjYSorted.push_back(iter.second);
			tempSceneObjZSorted.push_back(iter.second);
			continue;
		}


		//sort
		//x axis 
		for (std::vector<Object*>::iterator it = tempSceneObjXSorted.begin(); it != tempSceneObjXSorted.end(); it++)
		{
			it = tempSceneObjXSorted.insert(it, iter.second);

			if (iter.second->pos.x < tempSceneObjXSorted[std::distance(std::begin(tempSceneObjXSorted), it)]->pos.x)
			{
				tempSceneObjXSorted.insert(it, iter.second);
				break;
			}

			if (it == tempSceneObjXSorted.end())
			{
				tempSceneObjXSorted.push_back(iter.second);
			}
		}

		//y axis 
		for (std::vector<Object*>::iterator it = tempSceneObjYSorted.begin(); it != tempSceneObjYSorted.end(); it++)
		{
			it = tempSceneObjYSorted.insert(it, iter.second);

			if (iter.second->pos.x < tempSceneObjYSorted[std::distance(std::begin(tempSceneObjYSorted), it)]->pos.x)
			{
				tempSceneObjYSorted.insert(it, iter.second);
			}

			if (it == tempSceneObjYSorted.end())
			{
				tempSceneObjYSorted.push_back(iter.second);
			}
		}

		//z axis 
		for (std::vector<Object*>::iterator it = tempSceneObjZSorted.begin(); it != tempSceneObjZSorted.end(); it++)
		{
			it = tempSceneObjZSorted.insert(it, iter.second);

			if (iter.second->pos.x < tempSceneObjZSorted[std::distance(std::begin(tempSceneObjZSorted), it)]->pos.x)
			{
				tempSceneObjZSorted.insert(it, iter.second);
			}

			if (it == tempSceneObjZSorted.end())
			{
				tempSceneObjZSorted.push_back(iter.second);
			}

		}
	}


	//root node
	rootNode->data->minWorldSpace = glm::vec3(tempSceneObjXSorted[0]->pos.x, tempSceneObjYSorted[0]->pos.y, tempSceneObjZSorted[0]->pos.z);
	rootNode->data->maxWorldSpace = glm::vec3(tempSceneObjXSorted[sceneObjs.size()]->pos.x, tempSceneObjYSorted[sceneObjs.size()]->pos.y, tempSceneObjZSorted[sceneObjs.size()]->pos.z);
	constructTree(tempSceneObjXSorted, tempSceneObjYSorted, tempSceneObjZSorted, rootNode);
}


void AABBTREE::constructTree(std::vector<Object*>objXSorted, std::vector<Object*> objYSorted, std::vector<Object*> objZSorted, AabbNode* parentNode)
{

	if (objXSorted.size() == 1)
		return;

	std::array<float, 2> xAxisSahCost{}; //0: left, 1:right
	std::array<float, 2> yxisSahCost{};
	std::array<float, 2> zAxisSahCost{};

	//calc xAxisSahCost
	AabbNode* xAxisLeftNode = allocateAabbNode(glm::vec3(parentNode->data->minLocalSpace.x, parentNode->data->minWorldSpace.y, parentNode->data->minWorldSpace.z),
		glm::vec3(objXSorted[objXSorted.size() / 2]->aabb->centroidPos.x, parentNode->data->maxLocalSpace.y, parentNode->data->maxLocalSpace.z));

	xAxisSahCost[0] = sahCost(xAxisLeftNode->data, objXSorted);

	AabbNode* xAxisRightNode = allocateAabbNode(glm::vec3(objXSorted[objXSorted.size() / 2]->aabb->centroidPos.x, parentNode->data->minWorldSpace.y, parentNode->data->minWorldSpace.z),
		glm::vec3(parentNode->data->maxWorldSpace.x, parentNode->data->maxWorldSpace.y, parentNode->data->maxWorldSpace.z));

	xAxisSahCost[1] = sahCost(xAxisRightNode->data, objXSorted);


	//calc yAxisSahCost
	AabbNode* yAxisLeftNode = allocateAabbNode(glm::vec3(parentNode->data->minLocalSpace.x, parentNode->data->minWorldSpace.y, parentNode->data->minWorldSpace.z),
		glm::vec3(parentNode->data->maxLocalSpace.x, objYSorted[objYSorted.size() / 2]->aabb->centroidPos.y, parentNode->data->maxWorldSpace.z));

	yxisSahCost[0] = sahCost(yAxisLeftNode->data, objYSorted);

	AabbNode* yAxisRightNode = allocateAabbNode(glm::vec3(parentNode->data->minLocalSpace.x, objYSorted[objYSorted.size() / 2]->aabb->centroidPos.y, parentNode->data->minWorldSpace.z),
		glm::vec3(parentNode->data->maxLocalSpace.x, parentNode->data->maxWorldSpace.y, parentNode->data->maxWorldSpace.z));

	yxisSahCost[1] = sahCost(yAxisRightNode->data, objXSorted);

	//calc zAxisSahCost
	AabbNode* zAxisLeftNode = allocateAabbNode(glm::vec3(parentNode->data->minLocalSpace.x, parentNode->data->minWorldSpace.y, parentNode->data->minWorldSpace.z),
		glm::vec3(parentNode->data->maxLocalSpace.x, parentNode->data->maxLocalSpace.y, objZSorted[objZSorted.size() / 2]->aabb->centroidPos.z));

	zAxisSahCost[0] = sahCost(zAxisLeftNode->data, objXSorted);

	AabbNode* zAxisRightNode = allocateAabbNode(glm::vec3(parentNode->data->minLocalSpace.x, parentNode->data->minWorldSpace.y, objZSorted[objZSorted.size() / 2]->aabb->centroidPos.z),
		glm::vec3(parentNode->data->maxLocalSpace.x, parentNode->data->maxWorldSpace.y, parentNode->data->maxWorldSpace.z));

	yxisSahCost[1] = sahCost(zAxisRightNode->data, objXSorted);


	auto recursiveCall = [&](Axes axisToDivide)
		{
			std::vector<Object*>objXSortedRightPart;
			std::vector<Object*>objYSortedRightPart;
			std::vector<Object*>objZSortedRightPart;

			switch (axisToDivide)
			{
			case x:
				std::copy(objXSorted.end() - objXSorted.size() / 2, objXSorted.end(), objXSortedRightPart.begin());
				objXSorted.erase(objXSorted.end() - objXSorted.size() / 2, objXSorted.end());

				//create left node
				objYSorted = objXSorted;
				objZSorted = objXSorted;
				std::sort(objYSorted.begin(), objYSorted.end(), compObjPosY);
				std::sort(objZSorted.begin(), objZSorted.end(), compObjPosZ);

				AABBTREE::constructTree(objXSorted, objYSorted, objZSorted, xAxisLeftNode);

				//create right node
				objYSortedRightPart = objXSortedRightPart;
				objZSortedRightPart = objXSortedRightPart;
				std::sort(objYSortedRightPart.begin(), objYSortedRightPart.end(), compObjPosY);
				std::sort(objZSortedRightPart.begin(), objZSortedRightPart.end(), compObjPosZ);

				AABBTREE::constructTree(objXSortedRightPart, objYSortedRightPart, objZSortedRightPart, xAxisRightNode);
				break;

			case y:
				std::copy(objYSorted.end() - objYSorted.size() / 2, objXSorted.end(), objYSortedRightPart.begin());
				objYSorted.erase(objYSorted.end() - objYSorted.size() / 2, objYSorted.end());

				//create left node
				objXSorted = objYSorted;
				objZSorted = objYSorted;
				std::sort(objXSorted.begin(), objXSorted.end(), compObjPosX);
				std::sort(objZSorted.begin(), objZSorted.end(), compObjPosZ);

				AABBTREE::constructTree(objXSorted, objYSorted, objZSorted, yAxisLeftNode);

				//create right node
				objXSortedRightPart = objYSortedRightPart;
				objZSortedRightPart = objYSortedRightPart;
				std::sort(objXSortedRightPart.begin(), objXSortedRightPart.end(), compObjPosX);
				std::sort(objZSortedRightPart.begin(), objZSortedRightPart.end(), compObjPosZ);

				AABBTREE::constructTree(objXSortedRightPart, objYSortedRightPart, objZSortedRightPart, yAxisRightNode);
				break;

			case z:
				std::copy(objZSorted.end() - objZSorted.size() / 2, objXSorted.end(), objZSortedRightPart.begin());
				objZSorted.erase(objZSorted.end() - objZSorted.size() / 2, objZSorted.end());

				//create left node
				objXSorted = objZSorted;
				objYSorted = objZSorted;
				std::sort(objXSorted.begin(), objXSorted.end(), compObjPosX);
				std::sort(objYSorted.begin(), objYSorted.end(), compObjPosY);

				AABBTREE::constructTree(objXSorted, objYSorted, objZSorted, zAxisLeftNode);

				//create right node
				objXSortedRightPart = objZSortedRightPart;
				objYSortedRightPart = objZSortedRightPart;
				std::sort(objXSortedRightPart.begin(), objXSortedRightPart.end(), compObjPosX);
				std::sort(objYSortedRightPart.begin(), objYSortedRightPart.end(), compObjPosY);

				AABBTREE::constructTree(objXSortedRightPart, objYSortedRightPart, objZSortedRightPart, zAxisRightNode);
				break;
			}
		};

	//prepare recursive call stuff like that 
	if (xAxisSahCost[0] + xAxisSahCost[1] > yxisSahCost[0] + yxisSahCost[1])
	{
		desallocateAabnode(yAxisLeftNode);

		if (xAxisSahCost[0] + xAxisSahCost[1] > zAxisSahCost[0] + zAxisSahCost[1]) //chose xAxis to divide left and right node 
		{
			desallocateAabnode(zAxisLeftNode);
			parentNode->left = xAxisLeftNode;
			parentNode->right = xAxisRightNode;

			recursiveCall(x);
		}

		else  //chose zAxis to divide left and right node 
		{
			desallocateAabnode(xAxisLeftNode);
			parentNode->left = zAxisLeftNode;
			parentNode->right = zAxisRightNode;

			recursiveCall(z);
		}
	}

	else
	{
		desallocateAabnode(xAxisLeftNode);

		if (yxisSahCost[0] + yxisSahCost[1] > zAxisSahCost[0] + zAxisSahCost[1]) //chose yAxis to divide left and right node 
		{
			desallocateAabnode(zAxisLeftNode);
			parentNode->left = yAxisLeftNode;
			parentNode->right = yAxisRightNode;

			recursiveCall(y);
		}

		else //chose zAxis to divide left and right node 
		{
			desallocateAabnode(yAxisLeftNode);
			parentNode->left = zAxisLeftNode;
			parentNode->right = zAxisRightNode;

			recursiveCall(z);
		}
	}
}

float AABBTREE::sahCost(AABB* box, std::vector<Object*> objs)
{
	float objVolumes{};

	for (auto const& obj : objs)
	{
		objVolumes += obj->aabb->volume;
	}

	return objVolumes / box->volume;
}
