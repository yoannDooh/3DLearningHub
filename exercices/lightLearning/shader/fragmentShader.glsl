#version 330 core
in vec3 normal;
in vec3 FragPos; 
in vec2 TextCoord;

out vec4 FragColor;

struct DirectLight { //Directional light 
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct PointLight {
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
	vec3 pos;
	vec3 direction;
	float innercutOff; //cos(angle) of inner cone angle
	float outercutOff; //cos(angle) outer cone angle 
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

uniform Material material;
uniform Light light;
uniform SpotLight spotLight;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float emmissionStrength;

void main()
{
	//direction vectors and normal vector
	vec3 normVec = normalize(normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 reflectDirection = reflect(-lightDir, normVec);

	//diffuse and specular 
	float diff = max(dot(normVec, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);


	//diffuse / specular / ambient /emmission final value 
	vec3 specular = light.specular * spec * texture(material.specular, TextCoord).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextCoord).rgb;
	vec3 ambient = light.ambient * texture(material.diffuse, TextCoord).rgb;
	vec3 emmision  = texture(material.emission, TextCoord).rgb*emmissionStrength;

	//spotlightIntensity
	float theta = dot( lightDir,normalize(spotLight.direction) );
	float epsilon = lightDir.innercutOff - lightDir.outercutOff;
	float intensity = clamp((theta - lightDir.outerCutOff) / epsilon, 0.0, 1.0);

	//lightAttenuation
	float distance = length(PointLight.position - FragPos);
	float attenuation = 1.0f / (PointLight.constant + PointLight.linearCoef*distance + PointLight.squareCoef*(distance * distance));


	vec3 result = ambient + diffuse * vec3(153.0f / 255.0f, 0.0f, 0.0f) + specular + emmision;
	FragColor = vec4(result, 1.0);
}
