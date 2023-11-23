 #version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
layout(location = 2) in vec2 aTextCoord;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TextCoord;

uniform float cubeEdge;
uniform vec3 centerCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 orbit;
mat4 newModel;


mat4 BuildTranslation(vec3 delta) //from https://stackoverflow.com/questions/33807535/translation-in-glsl-shader
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(delta, 1.0));
}

void setOrbit()
{
	float cubeEdge5Percent = cubeEdge/100*5 ;
	vec4 centerCoordAfterElipsTrans = orbit*vec4(centerCoord.xyz,1.0);
	
	if ( aPos.y == centerCoord.y - cubeEdge / 2) //bottomFace
	{
		if ( aPos.x == centerCoord.x - cubeEdge / 2 ) //leftEdge
		{
			if ( aPos.z == centerCoord.z - cubeEdge / 2) //topLeft
				newModel = BuildTranslation( vec3(-cubeEdge/2, -cubeEdge/2, -cubeEdge/2) )*model;

			else  //bottomLeft
				newModel = BuildTranslation( vec3(-cubeEdge/2, -cubeEdge/2, cubeEdge/2) )*model;
		}

		else  //rightEdge
		{
			if ( aPos.z == (centerCoord.z - cubeEdge / 2) ) //topLeft
				newModel = BuildTranslation( vec3(cubeEdge/2, -cubeEdge/2, -cubeEdge/2) )*model;

			else  //bottomLeft
				newModel = BuildTranslation( vec3(cubeEdge/2, -cubeEdge/2, cubeEdge/2) )*model;

		}
	}

	else //topFace 
	{
		if ( aPos.x == centerCoord.x - cubeEdge / 2 ) //leftEdge
		{
			if ( aPos.z  == centerCoord.z - cubeEdge / 2 ) //topLeft
				newModel = BuildTranslation( vec3(-cubeEdge/2, cubeEdge/2, -cubeEdge/2) )*model;

			else  //bottomLeft
				newModel = BuildTranslation( vec3(-cubeEdge/2, cubeEdge/2, cubeEdge/2) )*model;
		}

		else  //rightEdge
		{
			if ( aPos.z == centerCoord.z - cubeEdge / 2 ) //topLeft
				newModel = BuildTranslation( vec3(cubeEdge/2, cubeEdge/2, -cubeEdge/2) )*model;

			else  //bottomLeft
				newModel = BuildTranslation( vec3(cubeEdge/2, cubeEdge/2, cubeEdge/2) )*model;

		}

	}



}

void main()
{
	gl_Position = projection * view * model *vec4(aPos.xyz,1.0);
	ourColor = aColor+vec3(gl_Position.xyz); // set ourColor to input color from the vertex data
	TextCoord = aTextCoord;
}

