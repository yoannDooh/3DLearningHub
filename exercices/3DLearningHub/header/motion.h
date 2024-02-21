#pragma once

#define POINT_LIGHTS_NB 2
#define DIRECT_LIGHTS_NB 1

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../header/shader.h"
#include "../header/mesh.h"
#include "../header/window.h"


//forward declaration class and struct
class Cube;
class Square;
namespace Light
{
	struct Material;
	struct DirectLight;
	struct lightPoint;
	struct SpotLight;
}

enum Effects
{
	inverse, //inverse color
	greyscale,// B&W color

	//Kernel effects
	blur,
	edgeDetection,
};

class FrameBuffer
{
public:
	unsigned int id{};
	unsigned int texId{};
	unsigned int renderId{};

	FrameBuffer(bool activateBufferTex, bool activateRenderBuff);
	FrameBuffer() {} 

protected:
	void genFrameBuffTex(int width, int height);
	void genRenderBuff();
};

class ShadowBuffer : public FrameBuffer
{
public:
	ShadowBuffer() {}
	unsigned int texId{};

	glm::mat4 depthMapLightSpaceMat{};
	std::array<glm::mat4,6> cubeMapLightSpaceMat{}; // in order : +x,-x,+y,-y,+z,-z

	unsigned int SHADOW_WIDTH{};
	unsigned int SHADOW_HEIGHT{};

	void genDepthMapBuff();
	void genDepthMapTex(int width, int height);
	void genDepthMapLightSpaceMat(float lightRange, glm::vec3 lightPos, glm::vec3 lookAtLocation);

	
	void genCubeMapBuff();
	void genCubeMapTex(int width, int height);
	void genCubeMapLightSpaceMat(float lightRange, glm::vec3 lightPos);
};


/*CAMERA AND MOUSE*/
class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float speed)
	{
		this->pos = pos;
		this->front = front;
		this->up = up;
		this->speed = speed;
	}

	void setCameraPos(glm::vec3 pos)
	{
		this->pos = pos;// set employees's name
	}

	void setCameraFront(glm::vec3 front)
	{
		this->front = front;// set employees's name
	}

	void setCameraUp(glm::vec3 up)
	{
		this->up = up;// set employees's name
	}


	void moveCamera(char keyPressed, float deltaTime)
	{
		switch (keyPressed)
		{
		case 'z':
			this->pos += this->speed * deltaTime * this->front;
			break;

		case 'q':
			this->pos -= glm::normalize(glm::cross(this->front, this->up)) * this->speed * deltaTime;
			break;

		case 's':
			this->pos -= this->speed * deltaTime * this->front;
			break;

		case 'd':
			this->pos += glm::normalize(glm::cross(this->front, this->up)) * this->speed * deltaTime;
			break;

		case 'u': //for up
			this->pos += this->speed * deltaTime * this->up;
			break;

		case 'w': //for down
			this->pos -= this->speed * deltaTime * this->up;
			break;

		}
		fillUbo0(2);
	}

	glm::vec3 pos{};
	glm::vec3 front{};
	glm::vec3 up{};
	float speed{};
	float yawAngle{ -90.0f };
	float pitchAngle{}; 
};

class Object
{
	public:
		glm::mat4 model{ glm::mat4(1.0f) };
		glm::mat4 localOrigin{ glm::mat4(1.0f) };
		glm::vec3 pos{}; //in world unit
		Light::lightPoint *lightPointPtr { nullptr };
		Light::SpotLight *spotLightPtr{ nullptr };

		bool outLine{ false };

		Object(){}
		Object(glm::vec3 pos);

		void move(glm::vec3 vector);
		void rotate(float rad,glm::vec3 rotateAxis);
		void scale(glm::vec3 scaleVec);
		void updateLightPoint(glm::vec3 newValue,int memberIndex); //memberIndex : 0 for color ... 4 for specular 
};

namespace Mouse
{
	extern float sensitivity;
	extern float lastX;
	extern float lastY;
	extern bool firstMouseInput;
	extern float fov;
}

/*FrameRate*/
namespace Time
{
	extern float deltaTime; // Time between current frame and last frame in sec
	extern float previousDelta; //Time of previous frame in sec
	extern float deltaSum; //summ of the seconds passed since the beggening (by adding all deltaTime)
	extern float currentFrameTime;
	extern float lastFrameTime;
	extern int sec; //how many entire seconds has passed
	extern int currentFrame;
	extern int totalFrame;
	extern float fps;
}

//GLOBAL VARIABLES
namespace World
{
	extern Camera camera;

	extern glm::mat4 view;
	extern glm::mat4 projection;

	extern float projectionWidth;
	extern float projectionHeight;
	extern float projectionNear;
	extern float projectionFar;



	extern std::vector<Object> objects;
	extern std::vector<Light::lightPoint> lightPoints;
	extern std::vector<Light::SpotLight> spotLights;
	extern std::array<Light::DirectLight, DIRECT_LIGHTS_NB> directLights;

	extern Object woodCube;
	extern std::array<Light::lightPoint, POINT_LIGHTS_NB> lightCube;
	extern std::array<Object, POINT_LIGHTS_NB> lightCubesObject;

	extern int mapWidth;
	extern int mapHeight;
}

/*LIGHT VARIABLES*/
namespace Light
{

	struct Material {
		std::array<float, 3> ambient;
		std::array<float, 3> diffuse{};
		std::array<float, 3> specular{};
		float shininess{};
	};

	struct DirectLight { //Directional light 
		glm::vec3 color{};
		glm::vec3 direction{};

		glm::vec3 ambient{};
		glm::vec3 diffuse{};
		glm::vec3 specular{};
	};

	struct lightPoint {
		glm::vec3 color{};
		glm::vec3 pos{};


		glm::vec3 ambient{};
		glm::vec3 diffuse{};
		glm::vec3 specular{};

		//attenuation variables
		float constant{};
		float linearCoef{};
		float squareCoef{};
	};

	struct SpotLight
	{
		glm::vec3 color{};
		glm::vec3 pos{};

		glm::vec3 direction{};
		float innercutOff{}; //cos(angle) of inner cone angle
		float outercutOff{}; //cos(angle) outer cone angle 

		glm::vec3 ambient{};
		glm::vec3 diffuse{};
		glm::vec3 specular{};

		//attenuation variables
		float constant{};
		float linearCoef{};
		float squareCoef{};
	};	
}

//lighting 
void setLighting();

//shadows functions
glm::mat4 toDirectionalLightSpaceMat(float lightRange, glm::vec3 lightPos, glm::vec3 lookAtLocation);
void setupShadowMap(ShadowBuffer depthMap);

//motion 
void rotatePlane(Object& object, double degree);
glm::mat4 orbit(Object& object, float horizontalAxis, float verticalAxis, float orbitDuration);  //orbitDuration is the number of seconds it takes for x to reach the value 2Py (so to do a 360)

//view & projection function
void updateViewProject(); //update world::view and world::projection

//emissionMap
float frameGlow(); //return currentFrameGlowStrenght for emmision map

//lightCube Object
void setLightCubes(Shader& shader, float cubeEdge);
void updateLightsCubePos();
void animateLightsCube(Shader& shader, Cube lightCubeVao);

//woodCube   
void setWoodCube(Shader& shader);//just translate and scale then pass to shader 
void animateWoodCube(Shader& shader,Cube woodCubeVao);

/*POST PROCESSING EFFECT*/
void animateWoodCubeAndOutline(Shader& woodBoxShader, Shader& outlineShader, Cube woodCubeVao);
void setEffect(Shader& shader, Effects effect);

//diverse useful function
glm::vec3 rgb(float red, float blue, float green);

float meterToWorldUnit(float meter); //1 meter equal 10 world unit

template <typename T> T degreeToRad(T);
