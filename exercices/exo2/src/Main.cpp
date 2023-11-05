#include "../header/shader.h"
#include "../header/window.h"
#include <cmath>
#include <chrono>
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
                        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,        // bottom left
                        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,       // bottom right
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



    //trans = glm::rotate(trans, glm::radians(360.0f), glm::vec3(1.0f, 0.0f, 0.0f));

     //setup rotation matrix 
    glm::mat4 model{ glm::mat4(1.0f) };
    glm::mat4 transModel{ glm::mat4(1.0f) };
    glm::mat4 rotModel{ glm::mat4(1.0f) };
    float bottomLeftCoord{ -0.5 };
    float fps{ 60 };
    float transPerFrame{ ( (1.0f-bottomLeftCoord)/2.0f) /fps }; 
    bool isItFirstLoop{ true };
   

 // render loop
 while (!glfwWindowShouldClose(window.windowPtr))
 {
    if (isItFirstLoop)
    {   
        int currentSec{ 0 };
        int count{ 1 };

        while (currentSec < 2)
        {
            int currentFrame{1};
            glfwSetTime(0);
            double t1{ glfwGetTime() };
            std::cerr << 1 / fps;
            while (currentFrame <= 60)
            {
                if (glfwGetTime() >= t1 + 1/fps)
                {   
                    //put to origin
                    if (currentFrame != 1)
                    {
                        model = glm::translate(model, glm::vec3(-transPerFrame * currentFrame, 0.0f, 0.0f));
                    }

                    //rotation matrix
                    rotModel = glm::rotate(transModel, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    
                   
                    //put to original position
                    if (currentFrame != 1)
                    {
                        model = glm::translate(model, glm::vec3(transPerFrame * currentFrame, 0.0f, 0.0f));
                    }
                  
                   //translation matrix
                    model = glm::translate(model, glm::vec3(transPerFrame, 0.0f, 0.0f));
                    model = rotModel * model;
                    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    
                    //background color
                    glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    shader.use();

                    //activate program object 
                    glBindVertexArray(VAO);
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                    glBindVertexArray(0);
                    glfwSwapBuffers(window.windowPtr);
                    glfwPollEvents();


                    ++currentFrame;
                    std::cout << count << '\n';
                    ++count;
                    t1 = glfwGetTime();
                 }    
             }  
            ++currentSec;
         }
        isItFirstLoop = false;
     }

     else
     {
        model = glm::translate(model, glm::vec3(-3, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        int currentSec{ 0 };
        int count{ 1 };
        while (currentSec < 4)
         {
            int currentFrame{ 1 };
            glfwSetTime(0);
            double t1{ glfwGetTime() };
            while (currentFrame <= 60)
            {
                if (glfwGetTime() >= t1 + 1 / fps)
                {

                    //put to origin
                    if (currentFrame != 1)
                    {
                        model = glm::translate(model, glm::vec3(-transPerFrame * currentFrame, 0.0f, 0.0f));
                    }

                    //rotation matrix
                    model = glm::rotate(model, glm::radians(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));


                    //put to original position
                    if (currentFrame != 1)
                    {
                        model = glm::translate(model, glm::vec3(transPerFrame * currentFrame, 0.0f, 0.0f));
                    }

                    //translation matrix
                    model = glm::translate(model, glm::vec3(transPerFrame, 0.0f, 0.0f));

                    if (currentFrame != 1)
                        //model = glm::translate(model, glm::vec3(0.0, -0.1f, 0.0f));
                        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

                    //background color
                    glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    shader.use();

                    //activate program object 
                    glBindVertexArray(VAO);
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                    glBindVertexArray(0);
                    glfwSwapBuffers(window.windowPtr);
                    glfwPollEvents();


                    ++currentFrame;
                    std::cout << count << '\n';
                    ++count;
                    t1 = glfwGetTime();
                }
            }
            ++currentSec;
         }
     }   
 }

 glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
