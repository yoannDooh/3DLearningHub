#version 330 core
#define M_PI 3.14

layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

uniform float radius;
uniform int pointsNb; //MUST BE GREATER THAN 3

/*
in VS_OUT{
	vec3 color;
} gs_in[];
*/

//https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103
void build_circle(vec4 position,int pointsNB,float radius)
{
	float angle= 360.0 / pointsNB;
	vec4 firstPoint = vec4(radius,0.0,0.0,1.0);
	vec4 lastPointPos;

	//create first triangle
	gl_Position = firstPoint; //point 0
	EmitVertex();
	
	gl_Position = vec4(radius * cos(radians(angle)), radius * sin(radians(angle)), 0.0, 1.0); //point 1
	EmitVertex();
	
	gl_Position = vec4(radius * cos(radians(2*angle)), radius * sin(radians(2*angle)), 0.0, 1.0); //point 2
	EmitVertex();

	for (int pointIndex = 3; pointIndex < pointsNB; ++pointIndex)
	{
		float currentAngle;
		lastPointPos = gl_Position;
		EndPrimitive();
		
		//put first point as first point of the new triangle
		gl_Position = firstPoint; 
		EmitVertex();
		
		//then the last point of the previous triangle 
		gl_Position = lastPointPos;
		EmitVertex();
	
		//create new point
		currentAngle = angle*pointIndex;
		gl_Position = vec4(radius * cos(radians(currentAngle)), radius * sin(radians(currentAngle)), 0.0, 1.0);
		EmitVertex();
	}
}


void main() 
{

	build_circle(gl_in[0].gl_Position,pointsNb,radius);
}
