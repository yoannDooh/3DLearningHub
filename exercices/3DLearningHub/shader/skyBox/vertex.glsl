#version 330 core
layout(location = 0) in vec3 inPos;

out vec3 TextCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TextCoord = inPos;
	vec4 pos = projection * view * vec4(inPos, 1.0);
	gl_Position = pos.xyww; //set the z value to w value so it fails all depth test 
}
