#version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
layout(location = 2) in vec2 aTextCoord;
layout(location = 3) in int cubePoint; 
	//in order : 
    //topLeft -> bottomLeft -> bottomRight -> topRight
    //bottom face ->  top face

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
	float halfCubeEdge = cubeEdge/2 ;
	vec4 centerCoordAfterElipsTrans = orbit*vec4(centerCoord.xyz,1.0);

	switch (cubePoint)
	{
		case 0 : //bottomFaceTopLeft
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x-halfCubeEdge, centerCoordAfterElipsTrans.y-halfCubeEdge, centerCoordAfterElipsTrans.z-halfCubeEdge) )*model;
		}

		case 1 : //bottomFaceBottomLeft
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x-halfCubeEdge, centerCoordAfterElipsTrans.y-halfCubeEdge, centerCoordAfterElipsTrans.z+halfCubeEdge) )*model;
		}
		
		case 2 : //bottomFaceBottomRight
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x+halfCubeEdge, centerCoordAfterElipsTrans.y-halfCubeEdge, centerCoordAfterElipsTrans.z+halfCubeEdge) )*model;
		}

		case 3 : //bottomFaceTopRight
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x+halfCubeEdge, centerCoordAfterElipsTrans.y-halfCubeEdge, centerCoordAfterElipsTrans.z-halfCubeEdge) )*model;
		}

		case 4 : //topFaceTopLeft
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x-halfCubeEdge, centerCoordAfterElipsTrans.y+halfCubeEdge, centerCoordAfterElipsTrans.z-halfCubeEdge) )*model;
		}
		
		case 5 : //topFaceBottomLeft
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x-halfCubeEdge, centerCoordAfterElipsTrans.y+halfCubeEdge, centerCoordAfterElipsTrans.z+halfCubeEdge) )*model;
		}

		case 6 : //topFaceBottomRight
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x+halfCubeEdge, centerCoordAfterElipsTrans.y+halfCubeEdge, centerCoordAfterElipsTrans.z+halfCubeEdge) )*model;
		}
		
		case 7 : //topFaceTopRight
		{
			newModel = BuildTranslation( vec3(centerCoordAfterElipsTrans.x+halfCubeEdge, centerCoordAfterElipsTrans.y+halfCubeEdge, centerCoordAfterElipsTrans.z-halfCubeEdge) )*model;
		}
	}
}

void main()
{
	setOrbit();
	gl_Position = projection * view * newModel *vec4(aPos.xyz,1.0);
	ourColor = aColor+vec3(gl_Position.xyz); // set ourColor to input color from the vertex data
	TextCoord = aTextCoord;
}

