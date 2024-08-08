#version 460 core

layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;


layout(std140, binding = 3) buffer aabbBuff
{
    int objId;
    int verticesNb;
    vec3 verticesCoords[];
};

uniform mat4 model;


void main()
{
    vec4 vertex = vec4(verticesCoords[gl_GlobalInvocationID.x], 1.0f);

    vertex = model * vertex;

    verticesCoords[gl_GlobalInvocationID.x] = vertex.xyz;
}