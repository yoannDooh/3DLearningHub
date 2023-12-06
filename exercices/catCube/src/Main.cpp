#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <complex>

namespace Math
{
    float py{ 3.14159 };
    double epsilon{ 1e-6 };
}
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float findYTransAmplitude(int frameNb, float translation) //frameNb : how many frame should the translation take to reach translation parameter value
{
    using namespace std::complex_literals;

    float result{};

    std::complex<float> operand1(0.0, Math::py / frameNb);
    operand1 = (2.0f * exp(operand1)) / (1.0f - exp(operand1));

    std::complex<float> operand2(0.0, -(Math::py / frameNb));
    operand2 = (2.0f * exp(operand2)) / (1.0f - exp(operand2));

    operand1 = operand1 - operand2;
    operand1 = (translation / operand1) * 2.0if;

    result = operand1.real();

    return result;
}

int main()
{
    float vertices[216]{};
    unsigned int indices[36]{};
    float cubeEdge{ 1.0f };


    std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft 

    std::array<Point, 8> points{};
    constructCube(vertices, indices, cubeEdge, cubeOriginCoord, points);

    glfwWindowHint(GLFW_SAMPLES, 4);
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl" };
    Shader shader(vertexPath, fragmentPath);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    shader.setFloat("cubeEdge", cubeEdge);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //point on cube attribute
    glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
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
    auto loadTexture = [&data, &width, &height](std::string TextureName)
        {
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
                glEnableVertexAttribArray(2);
                stbi_image_free(data);
            }

            else
                std::cout << "Failed to load : " << TextureName << std::endl;
        };

    //load texture
    loadTexture("catTexture");

    //grass texture
    unsigned int grassTex{};
    glGenTextures(1, &grassTex);
    glBindTexture(GL_TEXTURE_2D, grassTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("ressource\\grassTexture.jpg", &width, &height, &nrChannels, 0);

    //load texture
    loadTexture("grassTexture");

    shader.use();
    shader.setInt("catTexture", 0);
    shader.setInt("grassTexture", 1);


    // --ALL ANIMATIONS MATRIXES--
    glm::mat4 localOrigin{ glm::mat4(1.0f) };
    glm::mat4 yTransModel{ glm::mat4(1.0f) };
    glm::mat4 xyRotation{ glm::mat4(1.0f) };
    glm::mat4 ellipticOrbit{ glm::mat4(1.0f) };

    glm::vec4 aPointPos{ glm::vec4 ( -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), 1.0f ) };
    glm::vec4 aPoint { aPointPos };

    int fps{ 90 };
    float yTransModelVelocity{ 1 };

    // --VARIABLES FOR THE ANIMATION--

    //time/framerate variables
    float time{};
    int sec{};
    int currentFrame{ 1 };
    int totalFrame{ 1 };

    //elipses variables    
    float aValue{ 0.9f };
    float bValue{ 0.8f }; //b must be smaller or equal to a
    float elipsePerimeter { 2.0f * Math::py * sqrt((pow(aValue,2.0f) + pow(bValue,2.0f)) / 2.0f) };
    float xElipse { aValue };
    float yElipse { bValue };
    int orbitFrameNb { fps * 5 }; // how many frames should the animations takes
    int orbitFrameCurrentFrame { 1 };
    const int cubesNb { 10 }; //first cube is place to (0,b)
    float cubesSpacing { elipsePerimeter / cubesNb };  // a voir comment impl�menter //1 = doing a quarter of rotation around elipse (so 4 means doing a 360) ; by default the spacing is even between the number of cube

    //xY axis roation variables
    float xyRotationPerFrame{ 0.5f }; //how much radian it rotates by frame for the y value (x is the half of y value)

    //translation on y axis variables
    float yTrans{ 0.13f }; //by how much it should translate (from 0 to py etc...)
    int yTransFrameNb{ 90 }; // how many frames should the animations takes
    int yTransCurrentFrame{ 1 };
    float yTransAmplitude{ findYTransAmplitude(yTransFrameNb,yTrans) }; //amplitude of the sin function for yTranslation
    float yTransAmplitudeTimes2{ yTransAmplitude * 2 }; //amplitude of the sin function for yTranslation
    bool isItFirstLoop{ true };
    bool isAmplitudeNegative{ false };

    glm::vec3 barycenterCoord{};

    // --MODELS VIEX PROJECTION MATRIXES--

    //models
    glm::mat4 model{ glm::mat4(1.0f) };
    model = glm::translate(localOrigin, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //view
    glm::mat4 view{ glm::mat4(1.0f) };
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //projection
    glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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
 
    aPointPos = model*aPointPos;
    aPoint = aPointPos ;

    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        processInput(window.windowPtr);

        draw();
        currentFrame = 1;
        time = 0;
        glfwSetTime(0);


        while (currentFrame <= fps)
        {
            processInput(window.windowPtr);

            if (yTransCurrentFrame >= yTransFrameNb)
            {
                yTransCurrentFrame = 1;

                if (!isAmplitudeNegative)
                    isAmplitudeNegative = true;

                else
                    isAmplitudeNegative = false;
            }

            if (orbitFrameCurrentFrame >= orbitFrameNb)
            {
                orbitFrameCurrentFrame = 1;
            }

            if (isItFirstLoop)
            {
                yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, yTransAmplitude * sin((yTransCurrentFrame * Math::py / yTransFrameNb)), 0.0f));

                if (currentFrame + 1 >= fps)
                    isItFirstLoop = false;
            }

            else
            {
                if (isAmplitudeNegative)
                {
                    yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, -yTransAmplitudeTimes2 * sin((yTransCurrentFrame * Math::py / yTransFrameNb)), 0.0f));
                }

                else
                {
                    yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, yTransAmplitudeTimes2 * sin((yTransCurrentFrame * Math::py / yTransFrameNb)), 0.0f));
                }
            }

            xyRotation = glm::rotate(model, glm::radians(xyRotationPerFrame), glm::vec3(0.7f, 1.0f, 0.0f));

            xElipse = (aValue * cos(((2 * Math::py) / orbitFrameNb) * orbitFrameCurrentFrame));
            yElipse = (bValue * sin(((2 * Math::py) / orbitFrameNb) * orbitFrameCurrentFrame));
            ellipticOrbit = glm::translate(localOrigin, glm::vec3(xElipse, 0.0f, yElipse));


            model = yTransModel * xyRotation;

            aPoint = projection * view * model * aPointPos;

            barycenterCoord.x = aPoint.x + cubeEdge / 2;
            barycenterCoord.y = aPoint.y + cubeEdge / 2;
            barycenterCoord.z = aPoint.z + cubeEdge / 2;

            glUniform3f(glGetUniformLocation(shader.ID, "orbit"), barycenterCoord.x, barycenterCoord.y, barycenterCoord.z);
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr(ellipticOrbit));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));


            if (time == 0.0f)
            {
                draw();
                glfwSetTime(0);
                time = glfwGetTime();
                std::cerr << "Frame" << currentFrame << ") x : " << aPoint.x << " y : " << aPoint.y << " z : " << aPoint.z << " w : " << aPoint.w << "\n";
            }
                
            else
                if ( abs( time - (1/currentFrame) ) < Math::epsilon )
                {
                    draw();
                    std::cerr << "Frame" << currentFrame << ") x : " << aPoint.x << " y : " << aPoint.y << " z : " << aPoint.z << " w : " << aPoint.w << "\n";
                }

            ++yTransCurrentFrame;
            ++orbitFrameCurrentFrame;
            ++currentFrame;
            ++totalFrame;
        }
        ++sec;
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}