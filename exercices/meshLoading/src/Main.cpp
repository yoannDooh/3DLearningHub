#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>


namespace Mouse
{
    float sensitivity = 0.1f;
    float lastX{ 400 };
    float lastY{ 300 };
    bool firstMouseInput{ true };
    float fov{ 45 };
}

namespace Time
{
    float deltaTime{}; // Time between current frame and last frame in sec
    float previousDelta{}; //Time of previous frame in sec
    float deltaSum{}; //summ of the seconds passed since the beggening (by adding all deltaTime)
    float currentFrameTime{};
    float lastFrameTime{};
    int sec{}; //how many entire seconds has passed
    int currentFrame{ 1 };
    int totalFrame{ 1 };
}

void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f);


int main()
{
   

    glfwWindowHint(GLFW_SAMPLES, 4);
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl" };
    Shader shader(vertexPath, fragmentPath);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    stbi_set_flip_vertically_on_load(true);
    
    const char* modelPath{ ".\\rsc\\backpack\\backpack.obj" };
    Model ourModel(modelPath);

    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    
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
