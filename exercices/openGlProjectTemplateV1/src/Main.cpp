#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <complex>

namespace Mouse
{
    float sensitivity = 0.1f;
    float lastX{ 400 };
    float lastY{ 300 };
    bool firstMouseInput{ true };
    float fov{ 45 };
}

void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    /*
    float vertices[]{};
    unsigned int indices[]{};
    float cubeEdge{  };
    */

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
    //glGenBuffers(1, &VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    //texture
    //stbi_set_flip_vertically_on_load(true);

    /*
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
    */


    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /*
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveCamera('z', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.moveCamera('q', Time::deltaTime);


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveCamera('s', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.moveCamera('d', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.moveCamera('u', Time::deltaTime);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.moveCamera('w', Time::deltaTime);
    */
}

/*
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
*/