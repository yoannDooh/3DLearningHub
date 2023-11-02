#include "../header/shader.h"
#include "../header/window.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float vertices[]{
                        //coordinates           //colors
                        0.0f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,       // top 
                        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,        // bottom right
                        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,       // bottom left
                    };

unsigned int indices[]  { 
0, 1, 3, // first triangle
1, 2, 3// second triangle
};

int main()
{  
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl"};
    Shader shader(vertexPath, fragmentPath);

    // initialize VAO
    unsigned int VAO{};
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //initialize and bind GL_ARRAY_BUFFER 
    unsigned int VBO{};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //link position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //link color attribute 
    //link position attribute 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)( 3*sizeof(float)) );
    glEnableVertexAttribArray(1);

    //setup rotation matrix 
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

   
    // render loop
    bool isItFirstLoop{ true };
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        if (isItFirstLoop)
        { 
            shader.setFloat("U_offset", 0.5f);
            processInput(window.windowPtr);
            glfwSetTime(0);

            while (glfwGetTime() <= 2)
            {

                shader.setFloat("U_offset2", glfwGetTime());
               
                glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glUniformMatrix4fv(glGetUniformLocation(shader.ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
                
                shader.use();

                //activate program object 
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
                glfwSwapBuffers(window.windowPtr);
                glfwPollEvents();
                isItFirstLoop =false;
            }
        }

        else
        {
            shader.setFloat("U_offset", 1.5f);
            processInput(window.windowPtr);
            glfwSetTime(0);
            while (glfwGetTime() <= 3)
            {
                //glUniform1f(glGetUniformLocation(shader.ID, "U_angle"), glfwGetTime());
                shader.setFloat("U_offset2", glfwGetTime());
                glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glUniformMatrix4fv(glGetUniformLocation(shader.ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

                shader.use();

                //activate program object 
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
                glfwSwapBuffers(window.windowPtr);
                glfwPollEvents();
                isItFirstLoop = false;
            }
        }   
    }
    processInput(window.windowPtr);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
