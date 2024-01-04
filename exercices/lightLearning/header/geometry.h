#include <array>

struct Point
{
    std::array<float, 3> coord;     //vertices coord, in order : xyz
    std::array<float, 3> coolors;   //vertices coolors in order : rgb
    std::array<float, 3> normal;    //normal vector to the plane of the vertice 
};

void constructCubeD(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point);
void constructCubeN(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point); //construct cube but color is replaced by normals vector the face 

void constructCube(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point);
//in a row : first is coord then coloors then normal then texture coord the index of the point 
