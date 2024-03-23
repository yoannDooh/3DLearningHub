layout(triangle) in;
layout(line_strip, max_vertices = 6) out;

in vec3 normalVec;


const float length = 0.4;

uniform mat4 projection;

void GenerateLine(int index);

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}

void GenerateLine(int index)
{
    //first vertice of the line
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();

    //second vertice 
    gl_Position = projection * (gl_in[index].gl_Position + vec4(normalVec, 0.0) * length);
    EmitVertex();

    EndPrimitive();
}
