#include "../header/collision.h"


/*--AABB CLASS--*/
void AABB::updateMinMaxAfterRotation(std::vector<Vertex>& vertices, glm::vec3& orientation)
{
	glm::mat4 rotMat;

	for (int colIndex{}; colIndex < 3; ++colIndex)
	{
		rotMat[colIndex].w = 0.0f;
	}
	rotMat[3].x = 0.0f;
	rotMat[3].y = 0.0f;
	rotMat[3].z = 0.0f;
	rotMat[3].w = 1.0f;

	for (int axisIndex{}; axisIndex < 3; ++axisIndex)
	{
		float cos;
		float sin;

		switch (axisIndex)
		{
		case 1: //rotX
			cos = glm::cos(orientation.x);
			sin = glm::sin(orientation.x);

			rotMat[0].x = 1.0f;
			rotMat[0].y = 0.0f;
			rotMat[0].z = 0.0f;

			rotMat[1].x = 0.0f;
			rotMat[1].y = cos;
			rotMat[1].z = sin;


			rotMat[2].x = 0.0f;
			rotMat[2].y = -sin;
			rotMat[2].z = cos;
			break;


		case 2: //rotY
			cos = glm::cos(orientation.y);
			sin = glm::sin(orientation.y);

			rotMat[0].x = cos;
			rotMat[0].y = 0.0f;
			rotMat[0].z = 0.0f;

			rotMat[1].x = 0.0f;
			rotMat[1].y = cos;
			rotMat[1].z = sin;


			rotMat[2].x = 0.0f;
			rotMat[2].y = -sin;
			rotMat[2].z = cos;
			break;

		case 3:

			break;

		}
	}
}