#version 330 core
layout(location = 0) in vec3 aPos; // position has attribute position 0
layout(location = 1) in vec3 aColor; // color has attribute position 1
out vec3 ourColor; // output a color to the fragment shader*
uniform float U_offset;
uniform float U_offset2;
uniform float U_angle;
uniform mat4 trans;

mat2 rotate2d(float angle)
{
	return mat2(cos(angle),-sin(angle),sin(angle), cos(angle));
}

void main()
{
	//vec2 xyCoord = rotate2d(U_angle)*vec2(aPos.x-U_offset+U_offset2,-aPos.y);
	//gl_Position = vec4(xyCoord,aPos.z,1.0 );

	vec2 xyCoord = vec2(aPos.x-U_offset+U_offset2,-aPos.y);
	gl_Position = trans*vec4(xyCoord,aPos.z,1.0 );
	ourColor = aColor; // set ourColor to input color from the vertex data
}
