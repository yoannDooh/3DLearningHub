#include <array>

struct Point
{
    std::array<float, 3> coord;     //vertices coord, in order : xyz
    std::array<float, 3> coolors;   //vertices coolors in order : rgb
};

void constructCube(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point);
