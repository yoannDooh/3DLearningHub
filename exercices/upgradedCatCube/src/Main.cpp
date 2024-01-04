#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <cmath>
#include <complex>

struct Cube
{
    glm::mat4 model;
    glm::mat4 xyRotation;
    glm::mat4 yTransModel;
    float orbitStartingPos;
    float ytransStartingPos;
};

namespace Math
{
    float py{ 3.14159 };
    double epsilon{ 1e-6 };
}

namespace Time
{
    float deltaTime{}; // Time between current frame and last frame
    float previousDelta{};
    float deltaSum{}; // in sec 
    float deltaSumInMilliSec{}; //in milisec
    float currentFrameTime{};
    float lastFrameTime{};
    int sec{};
    int currentFrame{ 1 };
    int totalFrame{ 1 };
}

namespace Mouse
{
    float sensitivity = 0.1f;
    float lastX{ 400 };
    float lastY{ 300 };
    bool firstMouseInput{ true };
    float fov{ 45 };
}

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f);

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
    float cubeEdge{ 1.0f / 1.5 };


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

    glfwSetCursorPosCallback(window.windowPtr, mouse_callback);
    glfwSetScrollCallback(window.windowPtr, scroll_callback);



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
    glEnableVertexAttribArray(2);

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

    glm::mat4 shiftedlocalOrigin {
        glm::mat4(
            1.0f,0.0f,0.0f,0.0f,
            0.0f,cos(Math::py/3),sin(Math::py / 3),0.0f,
            0.0f,-sin(Math::py / 3),cos(Math::py / 3),0.0f,
            0.0f,0.0f,0.0f,1.0f
          )
    };

    glm::mat4 yTransModel{ glm::mat4(1.0f) };
    glm::mat4 xyRotation{ glm::mat4(1.0f) };
    glm::mat4 ellipticOrbit{ glm::mat4(1.0f) };

    glm::vec4 aPointPos{ glm::vec4(-(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), 1.0f) };
    glm::vec4 aPoint{ aPointPos };


    // --VARIABLES FOR THE ANIMATION--

    //elipses variables    
    float aValue{ 0.9f };
    float bValue{ 0.8f }; //b must be smaller or equal to a
    float xElipse{ aValue };
    float yElipse{ bValue };
    bool isRotationclockwise{ true };
    int rotationSens{ 1 }; //1 for clockwise -1 for counter clockwise 
    if (!rotationSens)
        rotationSens = -1;

    float orbitDuration{ 10 }; //the number of seconds it takes for x to reach the value 2Py (so to do a 360)
    float elipsePerimeter{ 2.0f * Math::py * sqrt((pow(aValue,2.0f) + pow(bValue,2.0f)) / 2.0f) };
    const int cubesNb{ 5 }; //first cube is place to (0,b)
    float cubesSpacingDistance{ elipsePerimeter / cubesNb };  // a voir comment implémenter //1 = doing a quarter of rotation around elipse (so 4 means doing a 360) ; by default the spacing is even between the number of cube


    //xY axis roation variables
    float xyRotationPerSec{ 30.0f }; //how much radian it rotates by frame for the y value (x is the half of y value)

    //translation on y axis variables
    float yTrans{ 2.0f }; //by how much it should translate from 0 to py  
    float yTransDuration{ 1000 };  //the number of Milisecond it takes for x to reach the value Py (so for the cube to starting from initial position reach it's max/min )
    float yTransDurationTimes2{ yTransDuration * 2 };
    int yTransCurrentSec{};
    float yTransAmplitude{ findYTransAmplitude(yTransDuration, yTrans) }; //amplitude of the sin function for yTranslation
    float yTransAmplitudeTimes2{ yTransAmplitude * 2 }; //amplitude of the sin function for yTranslation
    bool isItFirstLoop{ true };
    bool isAmplitudeNegative{ false };

    // --MODELS VIEX PROJECTION MATRIXES--

    //models
    glm::mat4 model{ glm::mat4(1.0f) };
    model = glm::translate(localOrigin, glm::vec3( 0.0f , 0.0f, 0.0f ) );
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //view
    glm::mat4 view{ glm::mat4(1.0f) };
    view = glm::lookAt(camera.pos,camera.pos+camera.front, camera.up );
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //projection
    glm::mat4 projection{ glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // init cubes variables
    std::array<Cube, cubesNb> cubes{};
    for (int cubeIndex { } ; cubeIndex < cubesNb; ++cubeIndex)
    {
        cubes[cubeIndex].model = model;
        cubes[cubeIndex].orbitStartingPos = ( cubesSpacingDistance / ( elipsePerimeter/ orbitDuration) )* (cubeIndex+1);
        cubes[cubeIndex].ytransStartingPos = (yTransDuration / cubesNb) * (cubeIndex + 1); 
    }


    auto prepareCube = [&shader, &texture, &grassTex, &VAO]()
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
        };

    auto swapBuffer = [&window]()
        {
            glBindVertexArray(0);
            glfwSwapBuffers(window.windowPtr);
            glfwPollEvents();
        };

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr(ellipticOrbit));

    // render loop
    glfwSetTime(0);

    float previousAmp{};
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        processInput(window.windowPtr);
        glfwSetInputMode(window.windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection{ glm::perspective(glm::radians(Mouse::fov), 800.0f / 600.0f, 0.1f, 100.0f) };
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


        prepareCube();

        //time etc..
        float yTranslationValue{};
        Time::currentFrameTime = glfwGetTime();
        Time::previousDelta = Time::deltaTime;
        Time::deltaTime = Time::currentFrameTime - Time::lastFrameTime;
        Time::lastFrameTime = Time::currentFrameTime;

        //drawing process      
        if ( yTransCurrentSec >= yTransDuration)
        {
            yTransCurrentSec = 0 ;
            isItFirstLoop = false;

            if (!isAmplitudeNegative)
                isAmplitudeNegative = true;

            else
                isAmplitudeNegative = false;
        }

        //yTranslation
        if (isItFirstLoop)
        {
            if (Time::currentFrame > 1 )
            {
                //ytransmodel = glm::translate(localorigin, glm::vec3(0.0f, ( ytransamplitude * sin( (math::py / ytransduration) * deltasuminmillisec ) ), 0.0f));
                for (auto& const cube : cubes)
                {
                    cube.yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, (yTransAmplitude * sin( (Math::py / yTransDuration) * ( /*cube.ytransStartingPos*/ +Time::deltaSumInMilliSec))), 0.0f));;
                }
            }
        }

        else
        {
            if (isAmplitudeNegative)
            {
               // yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, (-yTransAmplitudeTimes2 * sin((Math::py / yTransDurationTimes2) * deltaSumInMilliSec) ), 0.0f));

                for (auto& const cube : cubes)
                {
                    cube.yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, (-yTransAmplitudeTimes2 * sin( (Math::py / yTransDuration) * ( /*cube.ytransStartingPos*/ +Time::deltaSumInMilliSec))), 0.0f));
                }
            }

            else
            {
                //yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, (yTransAmplitudeTimes2 * sin((Math::py / yTransDurationTimes2) * deltaSumInMilliSec ) ), 0.0f));
                for (auto& const cube : cubes)
                {
                    cube.yTransModel = glm::translate(localOrigin, glm::vec3(0.0f, (yTransAmplitudeTimes2 * sin((Math::py / yTransDuration) * ( /*cube.ytransStartingPos*/ +Time::deltaSumInMilliSec) ) ), 0.0f));
                }   
            }
        }

        //xyRotation
        xyRotation = glm::rotate(model, glm::radians(xyRotationPerSec* Time::deltaTime), glm::vec3(0.7f, 1.0f, 0.0f));
        for (auto& const cube : cubes)
        {
            cube.xyRotation = glm::rotate(cube.model, glm::radians(xyRotationPerSec* Time::deltaTime), glm::vec3(0.7f, 1.0f, 0.0f));
        }
        


        //Elipse
        for (auto& const cube : cubes)
        {
           
            if (Time::deltaTime != 0)
            {                                                                                 
                xElipse = (aValue * cos( ( (2 * Math::py) / orbitDuration) * (cube.orbitStartingPos + Time::deltaSum) ) );
                yElipse = (bValue * sin( ( (2 * Math::py) / orbitDuration) * (cube.orbitStartingPos + Time::deltaSum) ) );
            }

            else
            {
                xElipse = (aValue * cos( 2 * Math::py  )  );
                yElipse = (bValue * sin( 2 * Math::py  )  );
            }

            
            ellipticOrbit = glm::translate(localOrigin, glm::vec3(xElipse * rotationSens, 0.0f, yElipse * rotationSens));
            cube.model = cube.yTransModel * cube.xyRotation;

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cube.model));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "orbit"), 1, GL_FALSE, glm::value_ptr(ellipticOrbit));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
       
        swapBuffer();

        Time::deltaSum = Time::deltaSum + Time::deltaTime;
        Time::deltaSumInMilliSec = Time::deltaSum * 1000;
        std::cerr << "Seconde : " << Time::sec << "\nFrame : " << Time::currentFrame << "\nNombre de Frames total : " << Time::totalFrame << "\nvaleur de delta : " << Time::deltaTime <<" secondes" <<"\ndeltaSum : " << Time::deltaSum << " secondes"
        << "\nisAmplitudeNegative : " << isAmplitudeNegative <<"\nyTransCurrentSec : " << yTransCurrentSec << "\n\n";

        if ( glfwGetTime() >= Time::sec+1)
        {
            ++Time::sec;
            ++yTransCurrentSec;
            Time::currentFrame = 1;
        }

        
        if (yTransCurrentSec > yTransDuration)
        {
            yTransCurrentSec = 0;
        }

        ++Time::currentFrame;
        ++Time::totalFrame;
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS  )
        camera.moveCamera('z',Time::deltaTime);
      
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.moveCamera('q', Time::deltaTime);

        
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS )
        camera.moveCamera('s', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.moveCamera('d', Time::deltaTime);

    if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS )
        camera.moveCamera('u', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.moveCamera('w', Time::deltaTime);

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    double xOffset{ xPos - Mouse::lastX };
    double yOffset{ Mouse::lastY - yPos };// reversed: y ranges bottom to top


    if (Mouse::firstMouseInput)
    {
        Mouse::lastX = xPos;
        Mouse::lastY = yPos;
        Mouse::firstMouseInput = false;
    }

    Mouse::lastX = xPos;
    Mouse::lastY = yPos;

    xOffset *= Mouse::sensitivity;
    yOffset *= Mouse::sensitivity;

    camera.yawAngle += xOffset;
    camera.pitchAngle += yOffset;

    if (camera.pitchAngle > 89.0f)
        camera.pitchAngle = 89.0f;

    if (camera.pitchAngle < -89.0f)
        camera.pitchAngle = -89.0f;

    glm::vec3 direction{};

    direction.x = cos(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
    direction.y = sin(glm::radians(camera.pitchAngle));
    direction.z = sin(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
    camera.front = glm::normalize(direction);


}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Mouse::fov -= (float)yoffset;

    if (Mouse::fov < 1.0f)
        Mouse::fov = 1.0f;

    if (Mouse::fov > 45.0f)
        Mouse::fov = 45.0f;
}