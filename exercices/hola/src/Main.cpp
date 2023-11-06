#include "../header/stb_image.h"
#include "../header/shader.h"
#include "../header/window.h"
#include "../header/stb_image.h"
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
            //coordinates        //colors               //texture coord
            0.5f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,      1.0f, 1.0f,       // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,       // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,      0.0f, 0.0f,        // bottom left
            -0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 1.0f,      0.0f, 1.0f,        // top left

};

unsigned int indices[]{
0, 1, 3, // first triangle
1, 2, 3// second triangle
};

int main()
{
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl" };
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

    //EBO
    unsigned int EBO{};
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //link position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //link color attribute 
    //link position attribute 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

     //setup rotation matrix 
    glm::mat4 model{ glm::mat4(1.0f) };
    glm::mat4 transModel{ glm::mat4(1.0f) };
    glm::mat4 rotModel{ glm::mat4(1.0f) };
    float bottomLeftCoord{ -0.5 };
    float fps{ 60 }; 
    float transPerFrame{ ((1.0f - bottomLeftCoord) / 2.0f) / fps };
    bool isItFirstLoop{ true };

    //init texture 
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //texture filtering 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //prepare texture
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



    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        if (isItFirstLoop)
        {
            int currentSec{ 0 };
            int count{ 1 };

            while (currentSec < 2)
            {
                int currentFrame{ 1 };
                glfwSetTime(0);
                double t1{ glfwGetTime() };
                std::cerr << 1 / fps;
                while (currentFrame <= 60)
                {
                    if (glfwGetTime() >= t1 + 1 / fps)
                    {
 
                        //translation matrix
                        model = glm::translate(model, glm::vec3(transPerFrame, 0.0f, 0.0f));
                        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

                        //background color
                        glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        shader.use();

                        //activate program object
                        glBindVertexArray(VAO);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
                     
                        //translation matrix
                        model = glm::translate(model, glm::vec3(transPerFrame, 0.0f, 0.0f));
                        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));


                        //background color
                        glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        shader.use();

                        //activate program object 
                        glBindVertexArray(VAO);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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