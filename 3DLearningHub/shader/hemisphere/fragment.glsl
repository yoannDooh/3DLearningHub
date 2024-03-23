#version 460 core

in vec3 Normal;
in vec3 FragPos;
in vec2 UV;
in vec3 Color;

out vec4 FragColor;


uniform int currentPhase;
uniform float elaspedTime; //timeInGame (in minutes)
uniform sampler2D skybox;
uniform sampler2D cloud;


const float offset_x = 1.0f / 800;
const float offset_y = 1.0f / 600;

vec2 offsets[9] = vec2[]
(
    //left							//center				//right
    /*top*/			vec2(-offset_x, offset_y), vec2(0.0f, offset_y), vec2(offset_x, offset_y),
    /*center*/		vec2(-offset_x, 0.0f), vec2(0.0f, 0.0f), vec2(offset_x, 0.0f),
    /*bottom*/		vec2(-offset_x, -offset_y), vec2(0.0f, -offset_y), vec2(offset_x, -offset_y)
    );

float kernelBlur[9] = float[]
(
    1 / 16, 2 / 16, 1 / 16,
    2 / 16, 4 / 16, 2 / 16,
    1 / 16, 2 / 16, 1 / 16
    );

vec3 sampleFrag[9];

vec3 cloudText();
bool isItInRange(float fragAltitude, float fragAzimuth, vec2 areaAltitude, vec2 areaAzimuthRange);
vec2 calcUv(bool reverse, vec3 fragPos, vec2 areaAltitude, vec2 areaAzimuthRange);

void main()
{

    //FragColor = vec4(1.0,0.0, 0.0,1.0);

    vec3 normalRgb = Normal * 0.5 + 0.5;
    FragColor = vec4(normalRgb, 1.0);


    vec3 cloudColor = cloudText();
    vec3 skyColor = texture(skybox, UV).rgb;

    switch (currentPhase)
    {
        case 0: //dawn
            skyColor = mix(skyColor, Color, 0.45);
            break;

        case 1: //daytime
            skyColor = mix(skyColor, Color, 0.45);
            break;

        case 2: //sunset
            skyColor = mix(skyColor, Color, 0.35);
            break;

        case 3: //night
            skyColor = mix(skyColor, Color, 0.65);
            break;
    }

    if (cloudColor != vec3(0.0, 0.0, 0.0) )
    {
        switch (currentPhase)
        {
        case 0: //dawn
            cloudColor = mix(cloudColor, Color, 0.65);
            break;

        case 1: //daytime
            cloudColor = mix(cloudColor, Color, 0.65);
            break;

        case 2: //sunset
            cloudColor = mix(cloudColor, Color, 0.55);
            break;

        case 3: //night
            cloudColor = mix(cloudColor, Color, 0.85);
            break;
        }

        FragColor = vec4(mix(skyColor,cloudColor,0.7), 1.0);
    }

    else
        FragColor = vec4(skyColor.rgb, 1.0);

    //FragColor = vec4(Color, 1.0);

}

vec3 cloudText()
{
    vec4 cloudText;
    vec2 uv;
    
    float fragAltitude = acos(FragPos.y);
    float fragAzimuth = atan(FragPos.z, FragPos.x);
    
    if (fragAzimuth < 0)
    {
        fragAzimuth = -1 * fragAzimuth;
    }

    bool area1 = isItInRange(fragAltitude, fragAzimuth, vec2(radians(10), radians(30)), vec2(radians(0), radians(40)));
    bool area2 = isItInRange(fragAltitude, fragAzimuth, vec2(radians(40), radians(60)), vec2(radians(0), radians(40)));
    bool area3 = isItInRange(fragAltitude, fragAzimuth, vec2(radians(5), radians(25)), vec2(radians(150), radians(200)));
    bool area4 = isItInRange(fragAltitude, fragAzimuth, vec2(radians(30), radians(60)), vec2(radians(0), radians(250)));
    bool area5 = isItInRange(fragAltitude, fragAzimuth, vec2(radians(40), radians(60)), vec2(radians(0), radians(40)));

    if (area1)
    {
        uv = calcUv(true, FragPos, vec2(radians(10), radians(30)), vec2(radians(0), radians(40)));
    }


    if (!area1)
    {
        uv = calcUv(true, FragPos, vec2(radians(0), radians(30)), vec2(radians(0), radians(40)));
    }
 

    cloudText = texture(cloud, uv);

    if (cloudText.a < 0.3)
        return vec3(0.0,0.0,0.0);

    return cloudText.rgb;
}

bool isItInRange(float fragAltitude, float fragAzimuth,vec2 areaAltitude,vec2 areaAzimuthRange)
{
    if ( fragAltitude >= areaAltitude[0] && fragAltitude <= areaAltitude[1] )
    {
        if (fragAzimuth >= areaAzimuthRange[0] && fragAltitude <= areaAzimuthRange[1] )
            return true;

        return false;
    }
    
    return false;
}

vec2 calcUv(bool reverse,vec3 fragPos,vec2 areaAltitude, vec2 areaAzimuthRange)
{
    vec2 uv;
    if (reverse)
    {
        uv.x = 1 - (0.5 + asin(fragPos.x) / (areaAzimuthRange[0] - areaAzimuthRange[1] ) ); //abs ? 
        uv.y = 1 - (asin(fragPos.y) / (areaAltitude[1] - areaAltitude[0] ) ) ;
    }

    else
    {
        uv.x = 0.5 + asin(fragPos.x) / (areaAzimuthRange[0] - areaAzimuthRange[1]);
        uv.y = asin(fragPos.y) / (areaAltitude[1] - areaAltitude[0]);
    }

    return uv;
}

