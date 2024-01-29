#version 410 core

out vec4 FragColor;

uniform sampler2D shadowMap;
in vec2 textCoord;
in vec4 normalVec;
in float height;
in vec3 fragPos;
in vec4 fragPosLightSpace;


float calcShadow(vec4 fragPosLightSpace);

void main()
{

	vec3 baseTerrainFragColor = vec3(height, height, height);
    float shadow = calcShadow(fragPosLightSpace);
    

	FragColor = vec4(baseTerrainFragColor,1.0);
}

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
