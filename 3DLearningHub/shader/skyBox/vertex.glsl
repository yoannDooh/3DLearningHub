#version 330 core
layout(location = 0) in vec3 pos;

out vec3 TextCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TextCoord = pos;
	vec4 pos = projection * view * vec4(pos, 1.0);
	gl_Position = pos.xyww; //set the z value to w value so it fails all depth test 
}
