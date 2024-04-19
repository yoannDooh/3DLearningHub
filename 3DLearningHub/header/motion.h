#pragma once

#define POINT_LIGHTS_NB 2
#define DIRECT_LIGHTS_NB 1
#define USR_PARAMETERS_INFO_TO_PRINT_NB 4


#include <array>
#include <vector>
#include <map>
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
	none,
};

enum DayPhases
{
	dawn,
	daytime,
	sunset,
	night
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
		Mesh* mesh{};
		Shader shaderOutline{};
		glm::mat4 model{ glm::mat4(1.0f) };
		glm::mat4 localOrigin{ glm::mat4(1.0f) };
		glm::vec3 pos{}; //in world unit
		glm::vec3 basePos{}; //in world unit

		float materialShininess{};

		bool enableTranslation { true };
		bool enableRotation { true };
		bool enableScale { true };
		bool isGlowing{ false };
		bool isOutLined{ false };
		bool isOrbiting{ false };
		
		
		int id{-1};
		int lighPointIndex{ -1 };
		int	spotLightIndex{ -1 };

		int worldObjIndex {-1}; //index of the element inside the World::Objects vector
		int worldLighPointIndex {-1}; //index of the element inside the World::lightPoint vector
		int worldSpotLightIndex {-1}; //index of the element inside the World::SpotLight vector

		Object(){}
		Object(Mesh* mesh) { this->mesh = mesh; }
		Object(Mesh* mesh,glm::vec3 pos);

		bool isLightPointIdValid(int id);
		bool isSpotLightIdValid(int id);

		void move(glm::vec3 vector);
		void rotate(float rad,glm::vec3 rotateAxis);
		void scale(glm::vec3 scaleVec);
		void rotatePlane(float degree);
		void updateLightPoint(glm::vec3 newValue,int memberIndex); //memberIndex : 0 for color ... 4 for specular 

		void set(Shader& shader,glm::vec3 translationVec, glm::vec3 rotationAxis,float rotationDegree, glm::vec3 scaleVec);//call once, before the renderLoop

		void animate(Shader& shader, glm::vec3 translationVec, glm::vec3 rotationAxis, float rotationDegree, glm::vec3 scaleVec); //call every frame, inside the renderLoop

		void setLightPoint(glm::vec3 color, glm::vec3 ambiant, glm::vec3 diffuse, glm::vec3 specular, float constant, float linearCoef, float squareCoef);

		void setOuline(Shader& shaderOutline, glm::vec3 outlineColor,float outlineWeight)
		{
			Object::shaderOutline = shaderOutline;
			Object::outlineColor = outlineColor;
			Object::outlineWeight = outlineWeight;
		}

		void setGlow(glm::vec3 glowColor,float glowStrenghtMax, float glowDuration)
		{
			Object::glowColor = glowColor;
			Object::glowStrenghtMax = glowStrenghtMax;
			Object::glowDuration = glowDuration;
		}	

		void setOrbit(float distFromCenter,float orbitHorizontalAxis, float orbitVerticalAxis,float orbitDuration)
		{
			Object::distFromCenter = distFromCenter;
			Object::orbitHorizontalAxis = orbitHorizontalAxis;
			Object::orbitVerticalAxis = orbitVerticalAxis;
			Object::orbitDuration = orbitDuration;
		}

		void addToWorldObjects();

private :
	//outline parameters
	glm::vec3 outlineColor{};
	float outlineWeight{};


	//glow animation
	float glow();
	glm::vec3 glowColor{};
	float glowStrenghtMax{};
	float glowDuration{}; //Duration to reach max value of glow before returning to 0 

	//orbit animation
	glm::mat4 orbit();
	float distFromCenter{}; //distance from the center of objects to its vertices 
	float orbitHorizontalAxis{};
	float orbitVerticalAxis{};
	float orbitDuration{};
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
	struct Time
	{
		float sec{};
		int min{};
		int hour{};
		int day{};

		
		Time(int hour, int min)
		{
			Time::hour = hour;
			Time::min = min;
		}

		Time()
		{
		}
	};

	extern float deltaTime; // Time between current frame and last frame in sec
	extern float previousDelta; //Time of previous frame in sec
	extern float deltaSum; //summ of the seconds passed since the beggening (by adding all deltaTime)
	extern float currentFrameTime;
	extern float lastFrameTime;
	extern int sec; //how many entire seconds has passed
	extern int currentFrame;
	extern int totalFrame;
	extern float fps;
	extern Time timeInGame;
	extern DayPhases dayPhase;
	extern std::map<DayPhases, std::array<Time,2> > dayPhasesTime;
	extern int totalMinInGame;
	extern float timeAccelerator; //accelerate or slow time, change le nom
	extern float currentPhaseNextPhaseDist;
	extern float currentHourCurrentPhaseBaseHourDist;

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

	extern std::map<DayPhases, glm::vec3>sunLightColor;

	extern Object sunObj;
	extern glm::vec3 sunPos; //in a sphere of a radius 1
	extern float sunAzimuth;   //in degree //useless
	extern float sunAltitude; //in degree //useless

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

	struct lightPoint { //manque un l majuscule 
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

namespace UsrParameters //nom à changer 
{
	enum InfoOption
	{
		position, 
		eyeDirection,	
		time,
		fps	
	};

	extern Effects currentEffect;
	extern bool activateWireframe;
	extern std::map<InfoOption, bool> infoOptions;

}

//lighting 
void setLighting();

void updateTimeInGame(); 

//shadows functions
glm::mat4 toDirectionalLightSpaceMat(float lightRange, glm::vec3 lightPos, glm::vec3 lookAtLocation);
void setupShadowMap(ShadowBuffer depthMap);

//view & projection function
void updateViewProject(); //update world::view and world::projection

//emissionMap
float frameGlow(); //return currentFrameGlowStrenght for emmision map

void setEffect(Shader& shader, Effects effect);


//sun and time functions
void calcDirectLightAttrib();
Time::Time timeDist(Time::Time time1, Time::Time time2, bool sameDay);
float timeToHour(Time::Time time);

//diverse useful function
glm::vec3 rgb(float red, float blue, float green);

float meterToWorldUnit(float meter); //1 meter equal 10 world unit


