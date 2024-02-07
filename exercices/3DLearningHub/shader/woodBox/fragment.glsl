#version 330 core

in vec3 normal;
in vec3 FragPos; 
in vec2 TextCoord;
in vec4 FragPosLightSpace;

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

struct Material {
    sampler2D  texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    float shininess;
};

uniform samplerCube skyBox;
//uniform sampler2D shadowMap;
uniform Material material;
 uniform PointLight pointLights[POINT_LIGHTS_NB];
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform DirectLight sunLight;
uniform float emmissionStrength;
uniform vec3 emmissionColor;


float calcShadow(vec4 fragPosLightSpace);
vec3 calcDirLight(DirectLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
    //direction vectors and normal vector
    vec3 normVec = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 MinusviewDir = -viewDir;


    //float shadow = calcShadow(FragPosLightSpace);
    //float shadow = 0;

    //reflection 
    vec3 reflectDir = reflect(MinusviewDir, normVec);
    vec3 reflectColor = texture(skyBox, reflectDir).rgb;

    //refraction
    float ratio = 1.00 / 1.52;
    vec3 refractDir = refract(MinusviewDir,normVec,ratio);
    vec3 refractColor = texture(skyBox, refractDir).rgb;

    //DirectLight 
    vec3 lightning = calcDirLight(sunLight, normVec, viewDir);

    for (int index = 0; index < POINT_LIGHTS_NB; ++index)
        lightning += calcPointLight(pointLights[index], normVec, viewDir,FragPos);

    vec3 emission = emmissionStrength * texture(material.texture_emission1, TextCoord).rgb* emmissionColor;

    FragColor = vec4(lightning /* + reflectColor * 0.3 + refractColor * 0.3*/ + emission, 1.0);
   
}

vec3 calcDirLight(DirectLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);

    //simple phong spec
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TextCoord)) * light.color;
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TextCoord));
    return (ambient + diffuse + specular );
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);


    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linearCoef * distance + light.squareCoef * (distance * distance));

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TextCoord)) * light.color;
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TextCoord)) * light.color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);

}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linearCoef * distance + light.squareCoef * (distance * distance));

    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TextCoord)) * light.color;
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TextCoord)) * light.color;
    ambient *= attenuation*intensity;
    diffuse *= attenuation*intensity;
    specular *= attenuation*intensity;
    return (ambient + diffuse + specular);

}

/*
float calcShadow(vec4 fragPosLightSpace)
{
    // perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //map to [0,1] range as shadowMap
    projCoords = projCoords * 0.5 + 0.5;

    //retrieve depth of the closest element to the light at the coords of the fragment  
    float closestFragDepth = texture(shadowMap, projCoords.xy).r;
    float currentFragDepth = projCoords.r;

    // if the depth of the current fragment is not greater than closestFragDepth it means it's in the shadow 
    return currentFragDepth > closestFragDepth ? 1.0 : 0.0;
}
*/