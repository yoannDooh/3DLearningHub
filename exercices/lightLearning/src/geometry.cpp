#include "../header/geometry.h"


void constructCubeD(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point)//originCoord should be coordinates of the top left of the bottom face of the cube
{
    //point array in order :
    //topLeft -> bottomLeft -> bottomRight -> topRight
    //bottom face ->  top face

    int index{};
    for (float value : originCoord)
    {
        point[0].coord[index] = value;
        ++index;
    }

    for (int face{ 1 }; face <= 2; ++face)
    {
        if (face == 1)
        {
            for (int pointIndex{ 1 }; pointIndex <= 4; ++pointIndex)
            {
                switch (pointIndex)
                {
                case 1: //bottomLeft
                    point[1].coord[0] = point[pointIndex - 1].coord[0];
                    point[1].coord[1] = point[pointIndex - 1].coord[1];
                    point[1].coord[2] = cote + point[pointIndex - 1].coord[2];
                    break;

                case 2: //bottomRight
                    point[2].coord[0] = cote + point[pointIndex - 1].coord[0];
                    point[2].coord[1] = point[pointIndex - 1].coord[1];
                    point[2].coord[2] = point[pointIndex - 1].coord[2];
                    break;

                case 3: //topRight
                    point[3].coord[0] = point[pointIndex - 1].coord[0];
                    point[3].coord[1] = point[pointIndex - 1].coord[1];
                    point[3].coord[2] = point[pointIndex - 1].coord[2] - cote;
                    break;
                }
            }
            continue;
        }

        for (int pointIndex{ 0 }; pointIndex < 4; ++pointIndex)
        {
            switch (pointIndex)
            {

            case 0: //topLeft
                point[4].coord[0] = point[0].coord[0];
                point[4].coord[1] = cote + point[0].coord[1];
                point[4].coord[2] = point[0].coord[2];
                break;

            case 1: //bottomLeft
                point[5].coord[0] = point[1].coord[0];
                point[5].coord[1] = cote + point[1].coord[1];
                point[5].coord[2] = point[1].coord[2];
                break;

            case 2: //bottomRight
                point[6].coord[0] = point[2].coord[0];
                point[6].coord[1] = cote + point[2].coord[1];
                point[6].coord[2] = point[2].coord[2];
                break;

            case 3: //topRight
                point[7].coord[0] = point[3].coord[0];
                point[7].coord[1] = cote + point[3].coord[1];
                point[7].coord[2] = point[3].coord[2];
                break;
            }
        }
    }

    int pointIndex{ 0 };
    for (Point& pointValue : point)
    {
        int coolorIndex{};
        for (int coordIndex{ 2 }; coordIndex >= 0; --coordIndex)
        {
            if (pointValue.coord[coordIndex] == point[0].coord[coordIndex])
                pointValue.coolors[coolorIndex] = 0.0f;

            else
                pointValue.coolors[coolorIndex] = 1.0f;

            ++coolorIndex;
        }
        ++pointIndex;
    }

    pointIndex = 0;

    int verticeIndex{};
    auto assignValue = [&vertices, &point, &verticeIndex](int pointIndex, int facePoint)
        {
            //facePoint point on the face is : 0=topLeft; 1=bottomLeft; 2=bottomRight 3=topRight
            if (verticeIndex == 0)
                vertices[verticeIndex] = point[pointIndex].coord[0];
            else
                vertices[++verticeIndex] = point[pointIndex].coord[0];

            vertices[++verticeIndex] = point[pointIndex].coord[1];
            vertices[++verticeIndex] = point[pointIndex].coord[2];

            vertices[++verticeIndex] = point[pointIndex].coolors[0];
            vertices[++verticeIndex] = point[pointIndex].coolors[1];
            vertices[++verticeIndex] = point[pointIndex].coolors[2];


            switch (facePoint)
            {
            case 0:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            case 1:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 2:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 3:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            }

            vertices[++verticeIndex] = pointIndex;
        };

    while (verticeIndex < 287)
    {
        for (int faceindex{}; faceindex < 6; ++faceindex)
        {
            switch (faceindex)
            {
            case 0: //bottomFace
                for (int pointIndex{}; pointIndex < 4; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex);
                }
                break;

            case 1: //topFace
                for (int pointIndex{ 4 }; pointIndex < 8; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex - 4);
                }
                break;

            case 2: //frontFace
                assignValue(5, 0); //topFace bottomLeft
                assignValue(1, 1); //bottomFace bottomLeft
                assignValue(2, 2); //bottomFace bottomRight
                assignValue(6, 3); //topFace bottomRight
                break;

            case 3: //backFace
                assignValue(4, 0); //topFace topLeft
                assignValue(0, 1); //bottomFace topLeft
                assignValue(3, 2); //bottomFace topRight
                assignValue(7, 3); //topFace topRight
                break;

            case 4: //leftFace
                assignValue(4, 0); //topFace topLeft
                assignValue(0, 1); //bottomFace topLeft
                assignValue(1, 2); //bottomFace bottomLeft
                assignValue(5, 3); //topFace bottomLeft
                break;

            case 5: //rightFace
                assignValue(7, 0); //topFace topRight
                assignValue(3, 1); //bottomFace topRight
                assignValue(2, 2); //bottomFace bottomRight
                assignValue(6, 3); //topFace bottomRight
                break;
            }
        }
    }


    int indicesIndex{};
    int currentFace{ 0 };
    while (indicesIndex < 36)
    {
        for (int count{}; count < 3; ++count) //first triangle indice
        {
            if (count == 2)
                indices[indicesIndex] = (currentFace * 4) + count + 1;
            else
                indices[indicesIndex] = (currentFace * 4) + count;

            ++indicesIndex;
        }

        for (int count{ (currentFace * 4) + 1 }; count <= currentFace * 4 + 3; ++count) //first triangle indice
        {
            indices[indicesIndex] = count;
            ++indicesIndex;
        }

        if (indicesIndex % 6 == 0)
        {
            ++currentFace;
        }
    }
}

void constructCubeN(float vertices[216], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point) //construct cube but color is replaced by normal vector to the face of the vertice 
{
    //point array in order :
    //topLeft -> bottomLeft -> bottomRight -> topRight
    //bottom face ->  top face

    int index{};
    for (float value : originCoord)
    {
        point[0].coord[index] = value;
        ++index;
    }

    for (int face{ 1 }; face <= 2; ++face)
    {
        if (face == 1)
        {
            for (int pointIndex{ 1 }; pointIndex <= 4; ++pointIndex)
            {
                switch (pointIndex)
                {
                case 1: //bottomLeft
                    point[1].coord[0] = point[pointIndex - 1].coord[0];
                    point[1].coord[1] = point[pointIndex - 1].coord[1];
                    point[1].coord[2] = cote + point[pointIndex - 1].coord[2];
                    break;

                case 2: //bottomRight
                    point[2].coord[0] = cote + point[pointIndex - 1].coord[0];
                    point[2].coord[1] = point[pointIndex - 1].coord[1];
                    point[2].coord[2] = point[pointIndex - 1].coord[2];
                    break;

                case 3: //topRight
                    point[3].coord[0] = point[pointIndex - 1].coord[0];
                    point[3].coord[1] = point[pointIndex - 1].coord[1];
                    point[3].coord[2] = point[pointIndex - 1].coord[2] - cote;
                    break;
                }
            }
            continue;
        }

        for (int pointIndex{ 0 }; pointIndex < 4; ++pointIndex)
        {
            switch (pointIndex)
            {

            case 0: //topLeft
                point[4].coord[0] = point[0].coord[0];
                point[4].coord[1] = cote + point[0].coord[1];
                point[4].coord[2] = point[0].coord[2];
                break;

            case 1: //bottomLeft
                point[5].coord[0] = point[1].coord[0];
                point[5].coord[1] = cote + point[1].coord[1];
                point[5].coord[2] = point[1].coord[2];
                break;

            case 2: //bottomRight
                point[6].coord[0] = point[2].coord[0];
                point[6].coord[1] = cote + point[2].coord[1];
                point[6].coord[2] = point[2].coord[2];
                break;

            case 3: //topRight
                point[7].coord[0] = point[3].coord[0];
                point[7].coord[1] = cote + point[3].coord[1];
                point[7].coord[2] = point[3].coord[2];
                break;
            }
        }
    }

    int pointIndex{ 0 };
    for (Point& pointValue : point)
    {
        int coolorIndex{};
        for (int coordIndex{ 2 }; coordIndex >= 0; --coordIndex)
        {
            if (pointValue.coord[coordIndex] == point[0].coord[coordIndex])
                pointValue.coolors[coolorIndex] = 0.0f;

            else
                pointValue.coolors[coolorIndex] = 1.0f;

            ++coolorIndex;
        }
        ++pointIndex;
    }

    pointIndex = 0;

    int verticeIndex{};
    auto assignValue = [&vertices, &point, &verticeIndex](int pointIndex, int facePoint, int face)
        {
            //facePoint point on the face is : 0=topLeft; 1=bottomLeft; 2=bottomRight 3=topRight
            //face is : 0=bottomFamce; 1=topFace; 2=frontFace 3=backFace 4=leftFace 5= rightFace
            if (verticeIndex == 0)
                vertices[verticeIndex] = point[pointIndex].coord[0];
            else
                vertices[++verticeIndex] = point[pointIndex].coord[0];

            vertices[++verticeIndex] = point[pointIndex].coord[1];
            vertices[++verticeIndex] = point[pointIndex].coord[2];

            switch (face)
            {
            case 0 : //bottomFace
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = -1.0f;
                vertices[++verticeIndex] = 0.0f;
                break; 

            case 1: //topFace 
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;  
                break;

            case 2: //frontface
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                break;
                
            case 3: //backface
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = -1.0f;
                break;

            case 4: //leftface
                vertices[++verticeIndex] = -1.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;  
                break;

            case 5: //rightface
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;     
                break;
            }

            switch (facePoint)
            {
            case 0:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            case 1:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 2:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 3:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            }

            vertices[++verticeIndex] = pointIndex;
        };

    while (verticeIndex < 192)
    {
        for (int faceindex{}; faceindex < 6; ++faceindex)
        {
            switch (faceindex)
            {
            case 0: //bottomFace
                for (int pointIndex{}; pointIndex < 4; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex , 0);
                }
                break;

            case 1: //topFace
                for (int pointIndex{ 4 }; pointIndex < 8; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex - 4 , 1);
                }
                break;

            case 2: //frontFace
                assignValue(5, 0, 2); //topFace bottomLeft
                assignValue(1, 1, 2); //bottomFace bottomLeft
                assignValue(2, 2, 2); //bottomFace bottomRight
                assignValue(6, 3, 2); //topFace bottomRight
                break;

            case 3: //backFace
                assignValue(4, 0, 3); //topFace topLeft
                assignValue(0, 1, 3); //bottomFace topLeft
                assignValue(3, 2, 3); //bottomFace topRight
                assignValue(7, 3, 3); //topFace topRight
                break;

            case 4: //leftFace
                assignValue(4, 0, 4); //topFace topLeft
                assignValue(0, 1, 4); //bottomFace topLeft
                assignValue(1, 2, 4); //bottomFace bottomLeft
                assignValue(5, 3, 4); //topFace bottomLeft
                break;

            case 5: //rightFace
                assignValue(7, 0, 5); //topFace topRight
                assignValue(3, 1, 5); //bottomFace topRight
                assignValue(2, 2, 5); //bottomFace bottomRight
                assignValue(6, 3, 5); //topFace bottomRight
                break;
            }
        }
    }


    int indicesIndex{};
    int currentFace{ 0 };
    while (indicesIndex < 36)
    {
        for (int count{}; count < 3; ++count) //first triangle indice
        {
            if (count == 2)
                indices[indicesIndex] = (currentFace * 4) + count + 1;
            else
                indices[indicesIndex] = (currentFace * 4) + count;

            ++indicesIndex;
        }

        for (int count{ (currentFace * 4) + 1 }; count <= currentFace * 4 + 3; ++count) //first triangle indice
        {
            indices[indicesIndex] = count;
            ++indicesIndex;
        }

        if (indicesIndex % 6 == 0)
        {
            ++currentFace;
        }
    }
}

void constructCube(float vertices[288], unsigned int indices[36], float cote, std::array<float, 3>& originCoord, std::array<Point, 8>& point)
{
    //point array in order :
    //topLeft -> bottomLeft -> bottomRight -> topRight
    //bottom face ->  top face

    int index{};
    for (float value : originCoord)
    {
        point[0].coord[index] = value;
        ++index;
    }

    for (int face{ 1 }; face <= 2; ++face)
    {
        if (face == 1)
        {
            for (int pointIndex{ 1 }; pointIndex <= 4; ++pointIndex)
            {
                switch (pointIndex)
                {
                case 1: //bottomLeft
                    point[1].coord[0] = point[pointIndex - 1].coord[0];
                    point[1].coord[1] = point[pointIndex - 1].coord[1];
                    point[1].coord[2] = cote + point[pointIndex - 1].coord[2];
                    break;

                case 2: //bottomRight
                    point[2].coord[0] = cote + point[pointIndex - 1].coord[0];
                    point[2].coord[1] = point[pointIndex - 1].coord[1];
                    point[2].coord[2] = point[pointIndex - 1].coord[2];
                    break;

                case 3: //topRight
                    point[3].coord[0] = point[pointIndex - 1].coord[0];
                    point[3].coord[1] = point[pointIndex - 1].coord[1];
                    point[3].coord[2] = point[pointIndex - 1].coord[2] - cote;
                    break;
                }
            }
            continue;
        }

        for (int pointIndex{ 0 }; pointIndex < 4; ++pointIndex)
        {
            switch (pointIndex)
            {

            case 0: //topLeft
                point[4].coord[0] = point[0].coord[0];
                point[4].coord[1] = cote + point[0].coord[1];
                point[4].coord[2] = point[0].coord[2];
                break;

            case 1: //bottomLeft
                point[5].coord[0] = point[1].coord[0];
                point[5].coord[1] = cote + point[1].coord[1];
                point[5].coord[2] = point[1].coord[2];
                break;

            case 2: //bottomRight
                point[6].coord[0] = point[2].coord[0];
                point[6].coord[1] = cote + point[2].coord[1];
                point[6].coord[2] = point[2].coord[2];
                break;

            case 3: //topRight
                point[7].coord[0] = point[3].coord[0];
                point[7].coord[1] = cote + point[3].coord[1];
                point[7].coord[2] = point[3].coord[2];
                break;
            }
        }
    }

    int pointIndex{ 0 };
    for (Point& pointValue : point)
    {
        int coolorIndex{};
        for (int coordIndex{ 2 }; coordIndex >= 0; --coordIndex)
        {
            if (pointValue.coord[coordIndex] == point[0].coord[coordIndex])
                pointValue.coolors[coolorIndex] = 0.0f;

            else
                pointValue.coolors[coolorIndex] = 1.0f;

            ++coolorIndex;
        }
        ++pointIndex;
    }

    pointIndex = 0;

    int verticeIndex{};
    auto assignValue = [&vertices, &point, &verticeIndex](int pointIndex, int facePoint, int face)
        {
            //facePoint point on the face is : 0=topLeft; 1=bottomLeft; 2=bottomRight 3=topRight
            if (verticeIndex == 0)
                vertices[verticeIndex] = point[pointIndex].coord[0];
            else
                vertices[++verticeIndex] = point[pointIndex].coord[0];

            vertices[++verticeIndex] = point[pointIndex].coord[1];
            vertices[++verticeIndex] = point[pointIndex].coord[2];

            vertices[++verticeIndex] = point[pointIndex].coolors[0];
            vertices[++verticeIndex] = point[pointIndex].coolors[1];
            vertices[++verticeIndex] = point[pointIndex].coolors[2];

            switch (face) // to assign the normal value by faces
            {
            case 0: //bottomFace
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = -1.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 1: //topFace 
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 2: //frontface
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            case 3: //backface
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = -1.0f;
                break;

            case 4: //leftface
                vertices[++verticeIndex] = -1.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 5: //rightface
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                break;
            }

            switch (facePoint)
            {
            case 0:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            case 1:
                vertices[++verticeIndex] = 0.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 2:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 0.0f;
                break;

            case 3:
                vertices[++verticeIndex] = 1.0f;
                vertices[++verticeIndex] = 1.0f;
                break;

            }

            vertices[++verticeIndex] = pointIndex;
        };

    while (verticeIndex < 192)
    {
        for (int faceindex{}; faceindex < 6; ++faceindex)
        {
            switch (faceindex)
            {
            case 0: //bottomFace
                for (int pointIndex{}; pointIndex < 4; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex, 0);
                }
                break;

            case 1: //topFace
                for (int pointIndex{ 4 }; pointIndex < 8; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex - 4, 1);
                }
                break;

            case 2: //frontFace
                assignValue(5, 0, 2); //topFace bottomLeft
                assignValue(1, 1, 2); //bottomFace bottomLeft
                assignValue(2, 2, 2); //bottomFace bottomRight
                assignValue(6, 3, 2); //topFace bottomRight
                break;

            case 3: //backFace
                assignValue(4, 0, 3); //topFace topLeft
                assignValue(0, 1, 3); //bottomFace topLeft
                assignValue(3, 2, 3); //bottomFace topRight
                assignValue(7, 3, 3); //topFace topRight
                break;

            case 4: //leftFace
                assignValue(4, 0, 4); //topFace topLeft
                assignValue(0, 1, 4); //bottomFace topLeft
                assignValue(1, 2, 4); //bottomFace bottomLeft
                assignValue(5, 3, 4); //topFace bottomLeft
                break;

            case 5: //rightFace
                assignValue(7, 0, 5); //topFace topRight
                assignValue(3, 1, 5); //bottomFace topRight
                assignValue(2, 2, 5); //bottomFace bottomRight
                assignValue(6, 3, 5); //topFace bottomRight
                break;
            }
        }
    }


    int indicesIndex{};
    int currentFace{ 0 };
    while (indicesIndex < 36)
    {
        for (int count{}; count < 3; ++count) //first triangle indice
        {
            if (count == 2)
                indices[indicesIndex] = (currentFace * 4) + count + 1;
            else
                indices[indicesIndex] = (currentFace * 4) + count;

            ++indicesIndex;
        }

        for (int count{ (currentFace * 4) + 1 }; count <= currentFace * 4 + 3; ++count) //first triangle indice
        {
            indices[indicesIndex] = count;
            ++indicesIndex;
        }

        if (indicesIndex % 6 == 0)
        {
            ++currentFace;
        }
    }
}
//in a row : first is coord then coloors then normal then texture coord the index of the point 