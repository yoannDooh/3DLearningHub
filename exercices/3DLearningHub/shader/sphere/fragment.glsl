#version 460 core

in vec3 Normal;
in vec3 FragPos;


out vec4 FragColor;

#define POINT_LIGHTS_NB 2

struct DirectLight { //Directional light 
    vec3 color;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 color;
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //attenuation variables
    float constant;
    float linearCoef;
    float squareCoef;

};

struct SpotLight
{
    vec3 color;
    vec3 pos;
    vec3 direction;


    //attenuation variables
    float constant;
    float linearCoef;
    float squareCoef;


    float innerCutOff; //cos(angle) of inner cone angle
    float outerCutOff; //cos(angle) outer cone angle 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(std140, binding = 0) uniform camAndProject
{
    mat4 view;
    mat4 projection;
    vec4 viewPosition;
};

layout(std140, binding = 1) buffer pointLightBuff
{
    PointLight pointLights[];
};

layout(std140, binding = 2) uniform directLightBuff
{
    DirectLight sunLight;
};


void main()
{
    vec3 viewPos = viewPosition.xyz;

    //direction vectors and normal vector
    vec3 viewDir = normalize(viewPos - FragPos);

    //FragColor = vec4(1.0,0.0, 0.0,1.0);

    vec3 normalRgb = Normal * 0.5 + 0.5;
    FragColor = vec4(normalRgb, 1.0);

}