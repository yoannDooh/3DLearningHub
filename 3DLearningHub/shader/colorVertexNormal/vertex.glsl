#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

out vec3 normalVec;

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = view * model * vec4(inPos, 1.0);
    mat3 normalMat = mat3(transpose(inverse(view * model)));
    normalVec = normalize(vec3(vec4(normalMat * inNormal, 0.0)));
}