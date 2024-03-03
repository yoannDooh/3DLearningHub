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
uniform int activateShadow = 1;
uniform int activateParallaxMapping = 0;
uniform sampler2D shadowMap;
uniform float parallaxScale = 0.1;
uniform samplerCube cubeShadowMap;
uniform float pointLightFarPlane;
uniform Area area1;
uniform int chunkWidth;
uniform int chunkHeight;


vec2 parallaxMapping(Area area, vec2 textCoord, vec3 viewDir);
vec3 calcDirLight(Area area, DirectLight light, vec3 normal, vec3 viewDir, vec3 diffuseText, float directShadow);
vec3 calcPointLight(Area area, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText, float pointShadow);
vec3 calcSpotLight(Area area, SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText);
vec3 areaFragText(Area area, vec2 textCoord, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 baseColor,float shadow);
float calcDirectShadow(vec4 fragPosLightSpace, vec3 normal);
float calcPointShadow(vec3 fragPos, vec3 viewPos, PointLight light);

vec3 sampleOffsetDirections[20] = vec3[] //for pointShadow pcf
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);


void main()
{
    vec3 viewPos = viewPosition.xyz;

    //direction vectors and normal vector
    vec3 normVec = normalize(vec3(normalVec));
    vec3 viewDir = normalize(viewPos - fragPos);


    vec3 tangentFragPos = TBN * fragPos;
    vec3 tangentViewPos = TBN * viewPos;

    vec3 tangentViewDir = normalize(tangentFragPos - tangentViewPos);

    float directShadow;
    float pointShadow;

    if (activateShadow == 1)
    {
        //shadow = calcDirectShadow(FragPosLightSpace, normVec);
        pointShadow = calcPointShadow(fragPos,viewPos,pointLights[0]);
    }

    else
    {
        directShadow = 0.0;
        pointShadow = 0.0;
    }

    vec3 baseTerrainFragColor = vec3(Height, Height, Height);
    vec3 textColor;


    vec2 textCoord = TextCoord;

    
    if (activateParallaxMapping == 1)
    {
        textCoord = parallaxMapping(area1, TextCoord, viewDir);
    }

    //faudra transfomer les chunks en uniform buffer objects et faire un fonction qui parse tous les chunks pour faire le lightning  
    if (activateNormalMap == 1)
    {
        vec2 uv;

        uv.x = (fragPos.x - area1.xRange[0]) / (area1.xRange[1] - area1.xRange[0]);
        uv.y = (fragPos.z - area1.zRange[0]) / (area1.zRange[1] - area1.zRange[0]);

        vec3 normal = texture(area1.texture_normal1, uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);

        textColor = areaFragText(area1, textCoord, normal, viewDir, fragPos, baseTerrainFragColor, pointShadow);

        vec3 normalRgb = normal * 0.5 + 0.5;
        vec3 normVecRgb = normVec * 0.5 + 0.5;
        //FragColor = vec4(normalRgb, 1.0);
    }

    else
        textColor = areaFragText(area1, textCoord, normVec, viewDir, fragPos, baseTerrainFragColor, pointShadow);


    //FragColor = vec4(texture(area1.texture_diffuse1, textCoord).rgb,1.0);
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
            lightning += calcPointLight(area, pointLights[index], normal, viewDir, fragPos, diffuse, shadow);
        

        return lightning;
    }

    /*
    else
    {
        //transform area Coord from patch range to it's own [0,1] range tjr mal expliqu� mais l� c'est vraiment �a
        vec2 uv;

        uv.x = (fragPos.x - area.xRange[0]) / (area.xRange[1] - area.xRange[0]);
        uv.y = (fragPos.z - area.zRange[0]) / (area.zRange[1] - area.zRange[0]);

        vec3 diffuse = texture(area.texture_diffuse1, uv).rgb;

        vec3 lightning = calcDirLight(area, sunLight, normal, viewDir, diffuse, shadow);


        for (int index = 0; index < POINT_LIGHTS_NB; ++index)
            lightning += calcPointLight(area, pointLights[index], normal, viewDir, fragPos, diffuse, shadow);


        return lightning;
    }
    */

    return baseColor;
}

vec3 calcDirLight(Area area, DirectLight light, vec3 normal, vec3 viewDir, vec3 diffuseText,float directShadow)
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

    return (ambient+(1.0- directShadow)*(diffuse + specular ) ); //je sais pas pourquoi mais en depend de l'angle on voit pas l'ombre donc je met ambient dans la parenthese aussi
}

vec3 calcPointLight(Area area, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 diffuseText, float pointShadow)
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

    return ( ambient + (1.0 - pointShadow) * (diffuse + specular)); //je sais pas pourquoi mais en depend de l'angle on voit pas l'ombre donc je met ambient dans la parenthese aussi

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

float calcDirectShadow(vec4 fragPosLightSpace, vec3 normal)
{
    float shadow = 0.0;

    vec3 lightPos = vec3(-25.32f, 78.77f, -20.65f);

    vec3 lightDir = normalize(lightPos - fragPos);

    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0);

    // perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //create uv in [0,1] range 
    vec2 uv;
    uv.x = projCoords.x * 0.5 + 0.5;
    uv.y = projCoords.y * 0.5 + 0.5;

    //retrieve depth of the closest element to the light at the coords of the fragment  
    float currentFragDepth = projCoords.z * 0.5 + 0.5;
    float closestFragDepth = texture(shadowMap, uv).r;


    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

            shadow += currentFragDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;

   // return currentFragDepth - bias > closestFragDepth ? 1.0 : 0.0;

    /*
    // if the depth of the current fragment is not greater than closestFragDepth it means it's in the shadow
    float shadow = currentFragDepth > closestFragDepth ? 1.0 : 0.0;

    return shadow;
    */
}

float calcPointShadow(vec3 fragPos,vec3 viewPos,PointLight light) 
{

    vec3 fragToLightVec = fragPos - light.pos;

    float closestFragDepth = texture(cubeShadowMap, fragToLightVec).r;
    //FragColor = vec4(vec3(closestFragDepth), 1.0);

    if (closestFragDepth == 1.0)
        return 0.0;

    if (closestFragDepth < 1.0)
    {
        //PCF for pointSHadow 
        float currentFragDepth = length(fragToLightVec);
        float shadow = 0.0;
        float bias = 0.05;
        int samples = 20;
        float viewDist = length(viewPos - fragPos);
        float diskRadius = (1.0 + (viewDist / pointLightFarPlane)) / 25.0;

        for (int sampleIndex = 0; sampleIndex < samples; ++sampleIndex)
        {
            closestFragDepth = texture(cubeShadowMap, fragToLightVec + sampleOffsetDirections[sampleIndex] * diskRadius).r;

            closestFragDepth *= pointLightFarPlane; // undo mapping [0;1]
            if (currentFragDepth - bias > closestFragDepth)
                shadow += 1.0;

        }

        shadow /= float(samples);

        return shadow;  
    }

}

vec2 parallaxMapping(Area area, vec2 textCoord, vec3 viewDir)
{    
    float height = texture(area.texture_displacement1, textCoord).r;

    return textCoord - viewDir.xy * (height * parallaxScale);
}