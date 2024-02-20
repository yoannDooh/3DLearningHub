#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 pointShadowLightSpaceMat[6];

out vec4 FragPos;

void main()
{
    for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
    {
        gl_Layer = faceIndex;
        for (int triangleVertexIndex = 0; triangleVertexIndex < 3; ++triangleVertexIndex) 
        {
            FragPos = gl_in[triangleVertexIndex].gl_Position;
            gl_Position = pointShadowLightSpaceMat[faceIndex] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}