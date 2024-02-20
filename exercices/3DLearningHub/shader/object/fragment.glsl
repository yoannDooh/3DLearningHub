#version 460 core

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


uniform samplerCube skyBox;
uniform samplerCube cubeShadowMap;
uniform sampler2D shadowMap;
uniform Material material;
uniform SpotLight spotLight;
uniform float emmissionStrength;
uniform vec3 emmissionColor;
uniform int activateShadow = 0;


float calcShadow(vec4 fragPosLightSpace, vec3 normal);
vec3 calcDirLight(DirectLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, float shadow);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
    vec3 viewPos = viewPosition.xyz;
    //direction vectors and normal vector
    vec3 normVec = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 MinusviewDir = -viewDir;
    
    float shadow;
    if (activateShadow == 1)
        shadow = calcShadow(FragPosLightSpace, normVec);
    else
        shadow = 0.0;

    shadow = 0.0;

    //reflection 
    vec3 reflectDir = reflect(MinusviewDir, normVec);
    vec3 reflectColor = texture(cubeShadowMap, reflectDir).rgb;

    //refraction
    float ratio = 1.00 / 1.52;
    vec3 refractDir = refract(MinusviewDir,normVec,ratio);
    vec3 refractColor = texture(skyBox, refractDir).rgb;

    //DirectLight 
    vec3 lightning = calcDirLight(sunLight, normVec, viewDir,shadow);

    for (int index = 0; index < POINT_LIGHTS_NB; ++index)
        lightning += calcPointLight(pointLights[index], normVec, viewDir,FragPos,shadow);

    vec3 emission = emmissionStrength * texture(material.texture_emission1, TextCoord).rgb* emmissionColor;

    FragColor = vec4(lightning  + reflectColor * 0.3 + refractColor * 0.3 + emission, 1.0f);

}

vec3 calcDirLight(DirectLight light, vec3 normal, vec3 viewDir,float shadow)
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
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, float shadow)
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
    return (ambient + (1.0 - shadow) * (diffuse + specular));

}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, float shadow)
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
    return (ambient + (1.0 - shadow) * (diffuse + specular));

}

float calcShadow(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 lightPos = vec3(-300.0, 1530.0, 22.0);

    vec3 lightDir = normalize(lightPos - FragPos);

    float bias = max(0.0009 * (1.0 - dot(normal, lightDir)), 0.0);
    //bias = 0.0000009;

    // perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //create uv in [0,1] range 
    vec2 uv;
    uv.x = projCoords.x * 0.5 + 0.5;
    uv.y = projCoords.y * 0.5 + 0.5;

    //retrieve depth of the closest element to the light at the coords of the fragment  
    float currentFragDepth = projCoords.z * 0.5 + 0.5;
    float closestFragDepth = texture(shadowMap, uv).r;

    return currentFragDepth - bias > closestFragDepth ? 1.0 : 0.0;

    /*
    // if the depth of the current fragment is not greater than closestFragDepth it means it's in the shadow
    float shadow = currentFragDepth > closestFragDepth ? 1.0 : 0.0;

    return shadow;
    */
}