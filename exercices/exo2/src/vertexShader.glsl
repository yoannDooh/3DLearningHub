#version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
out vec3 ourColor; // output a color to the fragment shader*
uniform mat4 model;

void main()
{
	
	//vec2 xyCoord = vec2(aPos.x-U_offset+U_offset2,-aPos.y);
	gl_Position = model* vec4(aPos.x,aPos.y,aPos.z, 1.0);
	ourColor = aColor; // set ourColor to input color from the vertex data
}
