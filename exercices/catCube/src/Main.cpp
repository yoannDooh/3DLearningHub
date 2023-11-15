#include "../header/stb_image.h"
#include "../header/openGL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

void processInput(GLFWwindow* window);

struct Point
{
    std::array<float, 3> coord;     //vertices coord, in order : xyz
    std::array<float, 3> coolors;   //vertices coolors in order : rgb 
};

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float vertices2[]{ 
   
    //face 6 (ABCD) :
             -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,      0.0f, 1.0f,        // top left
             -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,      0.0f, 0.0f,       // bottom left
             0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,      1.0f, 1.0f,      // top right

    //face 5 (EHGF) :
             -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,        // top left
             -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 0.0f,      0.0f, 0.0f,       // bottom left
             0.5f, 0.5f, 0.5f,    1.0f, 1.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 1.0f,      1.0f, 1.0f,      // top right

    //face 1  (HBCG): 
            //coordinates        //colors               //texture coord
            -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 0.0f,      0.0f, 1.0f,        // top left
            -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,      0.0f, 0.0f,        // bottom left
            0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 1.0f,      1.0f, 0.0f,       // bottom right
            0.5f, 0.5f, 0.5f,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f,       // top right

    //face 2 (FDAE) :
            -0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,      0.0f, 1.0f,        // top left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,      0.0f, 0.0f,        // bottom left
            0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,      1.0f, 0.0f,       // bottom right
            0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 1.0f,       1.0f, 1.0f,       // top right

    //face 3 (HBAE) :
             -0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,      0.0f, 1.0f,        // top left
             -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,      0.0f, 0.0f,        // bottom left
             -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,      1.0f, 0.0f,       // bottom right
             -0.5f, 0.5f, 0.5f,    1.0f, 1.0f, 0.0f,      1.0f, 1.0f,       // top right
    
    //face 4 (GCDF) :
             0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 1.0f,      0.0f, 1.0f,        // top left
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,     0.0f, 0.0f,       // bottom left
             0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 1.0f,      1.0f, 1.0f,      // top right

   

     
};
unsigned int indices2[]{
0, 1, 3, // first triangle
1, 2, 3,// second triangle

4, 5, 7, // first triangle
5, 6, 7,// second triangle

8, 9, 11, // first triangle
9, 10, 11,// second triangle

12, 13, 15, // first triangle
13, 14, 15,// second triangle

16, 17, 19, // first triangle
17, 18, 19,// second triangle

20, 21, 23, // first triangle
21, 22, 23,// second triangle
};

void constructCube(float vertices[192], unsigned int indices[36],float cote, std::array<float,3>& originCoord)//originCoord should be coordinates of the top left of the bottom face of the cube 
{
    std::array<Point,8>point{};
    //in order :  topLeft -> bottomLeft -> bottomRight -> topRight
    //bottom face ->  top face 

    int index{};
    for (float value : originCoord)
    {
        point[0].coord[index] = value;
        ++index;
    }

    for (int face{1}; face <=2; ++face)
    {
        if (face==1)
        {
            for (int pointIndex{1};pointIndex<=4;++pointIndex)
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
   
    int pointIndex{0};
    for (Point& pointValue : point)
    {
        int coolorIndex{};
        for (int coordIndex{2}; coordIndex>=0;--coordIndex)
        {
            if (pointValue.coord[coordIndex]== point[0].coord[coordIndex])
                pointValue.coolors[coolorIndex] = 0.0f;

            else
                pointValue.coolors[coolorIndex] = 1.0f;

            ++coolorIndex; 
        }
        ++pointIndex;
    }

    pointIndex = 0;

    int verticeIndex{};
    auto assignValue = [&vertices,&point,&verticeIndex](int pointIndex,int facePoint)
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
    };

    while (verticeIndex < 192)
    {
        for (int faceindex{}; faceindex < 6; ++faceindex)
        {
            switch (faceindex)
            {
            case 0 : //bottomFace 
                for (int pointIndex{}; pointIndex < 4; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex);
                }
                break;

            case 1: //topFace 
                for (int pointIndex{4}; pointIndex < 8; ++pointIndex)
                {
                    assignValue(pointIndex, pointIndex-4);
                }
                break;

            case 2: //frontFace 
                assignValue(5,0); //topFace bottomLeft
                assignValue(1,1); //bottomFace bottomLeft
                assignValue(2,2); //bottomFace bottomRight
                assignValue(6,3); //topFace bottomRight
                break;

            case 3: //backFace 
                assignValue(4,0); //topFace topLeft
                assignValue(0,1); //bottomFace topLeft
                assignValue(3,2); //bottomFace topRight
                assignValue(7,3); //topFace topRight
                break;

            case 4: //leftFace 
                assignValue(4,0); //topFace topLeft
                assignValue(0,1); //bottomFace topLeft
                assignValue(1,2); //bottomFace bottomLeft
                assignValue(5,3); //topFace bottomLeft
                break;

            case 5: //rightFace 
                assignValue(7,0); //topFace topRight
                assignValue(3,1); //bottomFace topRight
                assignValue(2,2); //bottomFace bottomRight
                assignValue(6,3); //topFace bottomRight
                break;
            }
        }
    }


    int indicesIndex{};
    int currentFace{0};
    while (indicesIndex<36)
    {
        for (int count{}; count < 3;++count) //first triangle indice
        {
            if (count == 2)
                indices[indicesIndex] = (currentFace * 4) + count+1;
            else
                indices[indicesIndex] = (currentFace * 4) + count;

            ++indicesIndex;
        }

        for (int count{ (currentFace * 4) +1}; count <= currentFace * 4 + 3; ++count) //first triangle indice
        {
            indices[indicesIndex] = count;
            ++indicesIndex;
        }

        if ( indicesIndex%6==0)
        {
            ++currentFace;
        }
    }    
}

void test(const float vertices1[192], float vertices2[192], unsigned int indices1[36], unsigned int indice2[36] )
{
    
    for (int index{}; index < 192; ++index)
    {
        if (vertices2[index]!= vertices1[index])
        {
            std::cout << "vertices1 : " << vertices1[index] << " et vertices2 :" << vertices2[index]<< " a index : " << index << "\n";
        }
    }

    std::cout << "\n";

    for (int index{}; index < 36; ++index)
    {
        if (indices1[index] != indices2[index])
        {
            std::cout << "vertices diff a ligne : " << index << "\n";
        }
    }
}

int main()
{
    float vertices[192]{};
    unsigned int indices[36]{};
    std::array<float, 3> cubeOriginCoord { -0.5f, -0.5f, -0.5f };
    constructCube(vertices, indices, 1, cubeOriginCoord);

    glfwWindowHint(GLFW_SAMPLES, 4);
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl" };
    Shader shader(vertexPath, fragmentPath);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    //VAO
    unsigned int VAO{};
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //VBO
    unsigned int VBO{};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //EBO
    unsigned int EBO{};
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //cat texture 
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //texture filtering 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width{}, height{}, nrChannels{};
    unsigned char* data{ stbi_load("ressource\\catTexture.jpg",&width,&height,&nrChannels,0) };
    
    //load texture
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

            glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        stbi_image_free(data);
    }

    else
        std::cout << "Failed to load texture" << std::endl;

    //grass texture 
    unsigned int grassTex{};
    glGenTextures(1, &grassTex);
    glBindTexture(GL_TEXTURE_2D, grassTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("ressource\\grassTexture.jpg",&width,&height,&nrChannels,0) ;

    //load texture
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        stbi_image_free(data);
    }

    else
        std::cout << "Failed to load texture" << std::endl;

    shader.use();
    shader.setInt("catTexture", 0);
    shader.setInt("grassTexture", 1);

    //world coordinates
    glm::mat4 model{ glm::mat4(1.0f) };
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //view 
    glm::mat4 view{ glm::mat4(1.0f) };
    view = glm::translate(view, glm::vec3(-0.0f, 0.0f, -3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //projection 
    glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    //all animation 
    glm::mat4 localOrigin{ glm::mat4(1.0f) };
    glm::mat4 yTransModel{ glm::mat4(1.0f) };

    float topFaceTopLeftYCoord{ vertices[33] };
    float bottomFaceBottomLeftYCoord{ vertices[9] };


    float highestPoint{ topFaceTopLeftYCoord+0.02f};
    float lowestPoint{ bottomFaceBottomLeftYCoord - 0.02f };
    float fps{ 60 };
    float yTransModelVelocity{ 0.3 };
    float yTranstransPerFrame{ (highestPoint / fps) * yTransModelVelocity }; 
    
    bool isItFirstLoop{ true };

    auto draw = [&shader, &texture, &grassTex, &VAO, &window]()
        {
            //background color
            glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
            glEnable(GL_MULTISAMPLE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.use();

            //activate program object
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, grassTex);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glfwSwapBuffers(window.windowPtr);
            glfwPollEvents();
        };

    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    { 
        draw();

        int currentFrame{ 1 };
        glfwSetTime(0);
        double t1{ glfwGetTime() };
        while (currentFrame <= fps)
        {
            
            std::cerr << currentFrame << '\n';
            ++currentFrame;
            t1 = glfwGetTime();
        }


        
        while (topFaceTopLeftYCoord <= highestPoint)
        {
            int currentFrame{ 1 };
            glfwSetTime(0);
            double t1{ glfwGetTime() };            
            while (currentFrame <= fps)
            {
                if (glfwGetTime() >= t1 + 1 / fps)
                {
                    yTransModel = glm::translate(model, glm::vec3(0.0f, yTranstransPerFrame, 0.0f));
                    model = yTransModel;
                    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    draw();

                    topFaceTopLeftYCoord = yTransModelVelocity +topFaceTopLeftYCoord;
                    std::cerr << currentFrame << '\n';
                    ++currentFrame;
                    t1 = glfwGetTime();
                }
            }
        }
        topFaceTopLeftYCoord = highestPoint - 0.02f;

        while (bottomFaceBottomLeftYCoord >= lowestPoint )
        {
            int currentFrame{ 1 };
            glfwSetTime(0);
            double t1{ glfwGetTime() };
            while (currentFrame <= fps)
            {
                if (glfwGetTime() >= t1 + 1 / fps)
                {
                    yTransModel = glm::translate(model, glm::vec3(0.0f, -yTranstransPerFrame, 0.0f));
                    model = yTransModel;
                    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    draw();

                    bottomFaceBottomLeftYCoord = -yTransModelVelocity + bottomFaceBottomLeftYCoord;
                    ++currentFrame;
                    t1 = glfwGetTime();
                }
            }
        }

        bottomFaceBottomLeftYCoord = lowestPoint + 0.02f;
    }
    

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}