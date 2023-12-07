#include "../header/stb_image.h"
#include "../header/openGL.h"
#include "../header/geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <complex>

void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f);

int main()
{
    
    float vertices[216]{};
    unsigned int indices[36]{};
    float cubeEdge{1.0f};
    std::array<Point, 8> points{};
    std::array<float, 3> cubeOriginCoord{ -(cubeEdge / 2.0f), -(cubeEdge / 2.0f), -(cubeEdge / 2.0f) }; //bottomFace topLeft

    constructCube(vertices, indices,cubeEdge,cubeOriginCoord,points);
    

    glfwWindowHint(GLFW_SAMPLES, 4);
    Window window(SCR_WIDTH, SCR_HEIGHT, "lightTraining");
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

  
    // --MODELS VIEX PROJECTION MATRIXES-- 
     glm::mat4 localOrigin{ glm::mat4(1.0f) };
    
     //models
    glm::mat4 model{ glm::mat4(1.0f) };
    model = glm::translate(localOrigin, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //view
    glm::mat4 view{ glm::mat4(1.0f) };
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //projection
    glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    auto draw = [&shader, &VAO, &window]()
        {
            //background color
            glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
            glEnable(GL_MULTISAMPLE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.use();

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glfwSwapBuffers(window.windowPtr);
            glfwPollEvents();
        };

    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        processInput(window.windowPtr);
        draw();

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