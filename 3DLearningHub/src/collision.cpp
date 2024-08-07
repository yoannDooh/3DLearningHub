#include "../header/collision.h"


/*--AABB CLASS--*/
void AABB::searchMinMax(const std::vector<Vertex>& vertices)
{

	for (int index{}; index < vertices.size(); ++index)
	{
		if (vertices[index].coord.x < min.x)
			min.x = vertices[index].coord.x;

		if (vertices[index].coord.y < min.y)
			min.y = vertices[index].coord.y;

		if (vertices[index].coord.z < min.z)
			min.z = vertices[index].coord.z;

		if (vertices[index].coord.x > max.x)
			max.x = vertices[index].coord.x;

		if (vertices[index].coord.y > max.y)
			max.y = vertices[index].coord.y;

		if (vertices[index].coord.z > max.z)
			max.z = vertices[index].coord.z;
	}
}