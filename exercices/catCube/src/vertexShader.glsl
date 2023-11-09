 #version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
layout(location = 2) in vec2 aTextCoord;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TextCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model *vec4(aPos.xyz,1.0);
	ourColor = aColor+vec3(gl_Position.xyz); // set ourColor to input color from the vertex data
	TextCoord = aTextCoord;
}

