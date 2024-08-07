#pragma once

#include "../header/mesh.h"
#include "../header/motion.h"

//forward declaration
glm::vec3 rgb(float red, float blue, float green);


class AABB
{
public:
	glm::vec3 min{};
	glm::vec3 max{};
	glm::vec3 color{ rgb(0,0,255) };

	Cube* cube;
	glm::mat4 matModel{ glm::mat4(1.0f) }; //should be named matModel
	glm::mat4 localOrigin{ glm::mat4(1.0f) };
	glm::vec3 pos{}; //in world unit

	void updateMinMaxAfterRotation(std::vector<Vertex>& vertices, glm::vec3& orientation);

	AABB() {}


};