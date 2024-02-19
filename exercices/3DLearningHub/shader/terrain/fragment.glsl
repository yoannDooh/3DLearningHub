#version 460 core

in vec2 TextCoord;
in vec4 normalVec;
in float Height;
in vec3 fragPos;
in vec4 FragPosLightSpace;
in mat3 TBN;

out vec4 FragColor;

#define MAX_POINT_LIGHTS_NB 200

#define POINT_LIGHTS_NB 2


struct Area {

    int chunkId;
    float[2] xRange; //first is xPosLEft and second xPosRight in WORLD unit, to indicate where the texture expand and until where it stretch 
    float[2] zRange; //same with z 
    float[2] yRange; //range in height of the texture

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_displacement1;
    sampler2D texture_emission1;
    float shininess;
    float specularIntensity;
};

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

uniform int chunkId;
uniform int activateNormalMap = 1;
uniform int activatShadow = 1;
uniform sampler2D shadowMap;
uniform Area area1;
uniform int chunkWidth;
uniform int chunkHeight;


vec3 calcDirLight(Area area, DirectLight light, vec3 normal, vec3 viewDir, vec3 diffuseText, float shadow);
vec3 calcPointLight(Area area, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText);
vec3 calcSpotLight(Area area, SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText);
vec3 areaFragText(Area area, vec2 textCoord, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 baseColor,float shadow);
float calcShadow(vec4 fragPosLightSpace, vec3 normal);

void main()
{
    vec3 viewPos = viewPosition.xyz;

    //direction vectors and normal vector
    vec3 normVec = normalize(vec3(normalVec));
    vec3 viewDir = normalize(viewPos - fragPos);

    float shadow;
    if (activatShadow == 1)
        shadow = calcShadow(FragPosLightSpace, normVec);
    else
        shadow = 0;

    vec3 baseTerrainFragColor = vec3(Height, Height, Height);

    vec3 textColor;

    //faudra transfomer les chunks en uniform buffer objects et faire un fonction qui parse tous les chunks pour faire le lightning  
    if (activateNormalMap == 1)
    {
        vec2 uv;

        uv.x = (fragPos.x - area1.xRange[0]) / (area1.xRange[1] - area1.xRange[0]);
        uv.y = (fragPos.z - area1.zRange[0]) / (area1.zRange[1] - area1.zRange[0]);

        vec3 normal = texture(area1.texture_normal1, uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);

        textColor = areaFragText(area1, TextCoord, normal, viewDir, fragPos, baseTerrainFragColor,shadow);
    }
    
    else
        textColor = areaFragText(area1, TextCoord, normVec, viewDir, fragPos, baseTerrainFragColor, shadow);

    FragColor = vec4(textColor, 1.0);
   
}

vec3 areaFragText(Area area, vec2 textCoord, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 baseColor, float shadow)
{
    vec2 maxUvVertexPos = vec2(chunkWidth / 2, chunkHeight / 2);
    vec2 minUvVertexPos = vec2(-chunkWidth / 2, -chunkHeight / 2);

    //transform chunk Coord from patch range to it's own [0,1] range he j'ai trop mal expliqu� carr�ment en fait c'est m�me pas je vais faire jsp je raconte quoi
    float uMin = (area.xRange[0] + abs(minUvVertexPos.x)) / (maxUvVertexPos.x + abs(minUvVertexPos.x));
    float uMax = (area.xRange[1] + abs(minUvVertexPos.x)) / (maxUvVertexPos.x + abs(minUvVertexPos.x));

    float vMin = (area.xRange[0] + abs(minUvVertexPos.y)) / (maxUvVertexPos.y + abs(minUvVertexPos.y));
    float vMax = (area.xRange[1] + abs(minUvVertexPos.y)) / (maxUvVertexPos.y + abs(minUvVertexPos.y));

    if (((textCoord.x > uMin) && (textCoord.x < uMax)) && ((textCoord.y > vMin) && (textCoord.y < vMax))) //it's within the texture range
    {
        //transform area Coord from patch range to it's own [0,1] range tjr mal expliqu� mais l� c'est vraiment �a
        vec2 uv;

        uv.x = (fragPos.x - area.xRange[0]) / (area.xRange[1] - area.xRange[0]);
        uv.y = (fragPos.z - area.zRange[0]) / (area.zRange[1] - area.zRange[0]);

        vec3 diffuse = texture(area.texture_diffuse1, uv).rgb;

        vec3 lightning = calcDirLight(area, sunLight, normal, viewDir, diffuse, shadow);

        
        for (int index = 0; index < POINT_LIGHTS_NB; ++index)
            lightning += calcPointLight(area, pointLights[index], normal, viewDir, fragPos, diffuse);
        

        return lightning;
    }

    return baseColor;
}

vec3 calcDirLight(Area area, DirectLight light, vec3 normal, vec3 viewDir, vec3 diffuseText,float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0); 
    vec3 reflectDir = reflect(-lightDir, normal);

    //simple phong spec
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), area.shininess);

    //diffuse / specular / ambient 
    vec3 ambient = light.ambient * diffuseText;
    vec3 diffuse = light.diffuse * diff * diffuseText * light.color;
    vec3 specular = light.specular * spec * area.specularIntensity * light.color;

    return (ambient + (1.0-shadow)*(diffuse + specular ));
}

vec3 calcPointLight(Area area, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);


    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), area.shininess);

    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linearCoef * distance + light.squareCoef * (distance * distance));

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * diffuseText;
    vec3 diffuse = light.diffuse * diff * diffuseText * light.color;
    vec3 specular = light.specular * spec * light.color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);

}

vec3 calcSpotLight(Area area, SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    //diffuse
    float diff = max(dot(lightDir, normal), 0.0); //je comprend pas y a encore un blême avec les normals 

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), area.shininess);//je comprend pas y a encore un blême avec les normals 

    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linearCoef * distance + light.squareCoef * (distance * distance));

    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);


    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * diffuseText;
    vec3 diffuse = light.diffuse * diff * diffuseText * light.color;
    vec3 specular = light.specular * spec * light.color;

    return (ambient + diffuse + specular);

}

float calcShadow(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 lightPos = vec3(-300.0, 1530.0, 22.0);

    vec3 lightDir = normalize(lightPos - fragPos);

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