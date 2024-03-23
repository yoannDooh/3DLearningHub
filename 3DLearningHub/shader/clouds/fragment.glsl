#version 460 core

in vec3 FragPos;
in vec2 UV;
in vec3 Color;

out vec4 FragColor;

uniform sampler2D cloudTexture;
//uniform sampler2D skyTexture;


//day phases uniforms :
uniform int currentPhase;
uniform float currentPhaseNextPhaseDist;
uniform float currentHourCurrentPhaseBaseHourDist;


void main()
{

    vec4 cloudTex = texture(cloudTexture, UV).rgba;

    if (cloudTex.a < 0.1)
        discard;

    FragColor = vec4(cloudTex.rgb,1.0);
}
