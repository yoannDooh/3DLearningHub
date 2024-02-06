#version 410 core

in vec2 TextCoord;
in vec4 normalVec;
in float height;
in vec3 fragPos;
in vec4 fragPosLightSpace;


out vec4 FragColor;

#define POINT_LIGHTS_NB 2

struct Chunk {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_displacement1;
    sampler2D texture_emission1;
    float shininess;

    float[2] xRange; //first is xPosLEft and second xPosRight in WORLD unit, to indicate where the texture expand and until where it stretch 
    float[2] zRange; //same with z 
    float[2] yRange; //range in height of the texture
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


uniform sampler2D shadowMap;
uniform PointLight pointLights[POINT_LIGHTS_NB];
uniform Chunk chunk1;
uniform vec3 viewPos;
uniform DirectLight sunLight;
uniform vec2 maxUvVertexPos;
uniform vec2 minUvVertexPos; //it should be negative since terrain extend from negative to positive so 0 is at the center 


vec3 calcDirLight(Chunk chunk,DirectLight light, vec3 normal, vec3 viewDir, vec3 diffuseText);
vec3 calcPointLight(Chunk chunk, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos,vec3 diffuseText);
vec3 calcSpotLight(Chunk chunk,SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText);
//vec3 calcNormal(Chunk chunk, vec2 uv);
vec3 chunkFragText(Chunk chunk, vec2 textCoord,vec3 normal,vec3 viewDir,vec3 fragPos, vec3 baseColor);
float calcShadow(vec4 fragPosLightSpace);

void main()
{
    //direction vectors and normal vector
    vec3 normVec = normalize(vec3(normalVec));
    vec3 viewDir = normalize(viewPos - fragPos);

    float shadow = calcShadow(fragPosLightSpace);
	vec3 baseTerrainFragColor = vec3(height, height, height);


    //faudra transfomer les chunks en uniform buffer objects et faire un fonction qui parse tous les chunks pour faire le lightning
    /*
    vec3 lightning = calcDirLight(chunk1,sunLight,normVec,viewDir);

    for (int index = 0; index < POINT_LIGHTS_NB; ++index)
        lightning += calcPointLight(chunk1,pointLights[index], normVec, viewDir, fragPos);
    */

    vec3 textColor = chunkFragText(chunk1, TextCoord, normVec, viewDir,fragPos, baseTerrainFragColor);

    vec3 diffuse = texture(chunk1.texture_diffuse1, TextCoord).rgb;
    FragColor = vec4(baseTerrainFragColor,1.0);
}

vec3 chunkFragText(Chunk chunk,vec2 textCoord,vec3 normal, vec3 viewDir ,vec3 fragPos, vec3 baseColor)
{
    //transform chunk Coord from patch range to it's own [0,1] range he j'ai trop mal expliqu� carr�ment en fait c'est m�me pas je vais faire jsp je raconte quoi
    float uMin = (chunk.xRange[0] + abs(minUvVertexPos.x) ) / ( maxUvVertexPos.x + abs(minUvVertexPos.x) ) ;
    float uMax = (chunk.xRange[1] + abs(minUvVertexPos.x) )/  ( maxUvVertexPos.x + abs(minUvVertexPos.x) ) ;


    float vMin = (chunk.xRange[0] + abs(minUvVertexPos.y)) / (maxUvVertexPos.y + abs(minUvVertexPos.y));
    float vMax = (chunk.xRange[1] + abs(minUvVertexPos.y)) / (maxUvVertexPos.y + abs(minUvVertexPos.y));
    
    if ( ( (textCoord.x > uMin) && (textCoord.x < uMax) ) && ( (textCoord.y > vMin ) && (textCoord.y < vMax) ) ) //it's within the texture range
    {
       //transform chunk Coord from patch range to it's own [0,1] range tjr mal expliqu� mais l� c'est vraiment �a
        vec2 uv;
        
        uv.x = (fragPos.x - chunk.xRange[0] ) / (chunk.xRange[1] - chunk.xRange[0]);
        uv.y = (fragPos.z - chunk.zRange[0]) / (chunk.zRange[1] - chunk.zRange[0]);

        vec3 diffuse = texture(chunk.texture_diffuse1, uv).rgb;

        vec3 lightning = calcDirLight(chunk, sunLight, normal, viewDir, diffuse);

        for (int index = 0; index < POINT_LIGHTS_NB; ++index)
            lightning += calcPointLight(chunk, pointLights[index], normal, viewDir, fragPos, diffuse);


        return lightning;
    }

    return baseColor;
}

vec3 calcDirLight(Chunk chunk,DirectLight light, vec3 normal, vec3 viewDir,vec3 diffuseText)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);

    //simple phong spec
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), chunk.shininess);

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * diffuseText;
    vec3 diffuse = light.diffuse * diff * diffuseText * light.color;


    vec3 specular = light.specular * spec ;
    return (ambient + diffuse + 0);
}

//JE COMPREND PAS CE QUE FAIT LE SPECULAR 
vec3 calcPointLight(Chunk chunk,PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);


    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), chunk.shininess);

    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linearCoef * distance + light.squareCoef * (distance * distance));

    //diffuse / specular / ambient final value
    vec3 ambient = light.ambient * diffuseText;
    vec3 diffuse = light.diffuse * diff *  diffuseText * light.color;
    vec3 specular = light.specular * spec  * light.color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + 0);

}

vec3 calcSpotLight(Chunk chunk, SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //Blinn spec
    float spec = pow(max(dot(normal, halfwayDir), 0.0), chunk.shininess);

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
    vec3 specular = light.specular * spec  * light.color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);

}

/*
vec3 calcNormal(Chunk chunk,vec2 uv)
{
    
    normal = texture(chunk.texture_normal1,uv).rgb;

    // transform to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);
   
}
*/

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
