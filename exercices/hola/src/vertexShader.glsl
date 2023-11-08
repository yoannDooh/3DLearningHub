 #version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
layout(location = 2) in vec2 aTextCoord;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TextCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 localModel;

void main()
{
	vec4 localModelCoord = localModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = projection * view * model * localModelCoord;
	ourColor = aColor+vec3(localModelCoord.xyz); // set ourColor to input color from the vertex data
	TextCoord = aTextCoord;
}

