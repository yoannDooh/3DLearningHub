#include "../header/stb_image.h"
#include "../header/openGL.h"
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


    //init texture cat
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //texture filtering 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //view 
    glm::mat4 view{ glm::mat4(1.0f) };
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    //projection 
    glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //local
    glm::mat4 localModel{ glm::mat4(1.0f) };

    //bottomRightCoord calculation
    glm::vec4 bottomRightCoord( vertices[8] , vertices[9], vertices[10], 1.0f );
    bottomRightCoord = projection* view* model* bottomRightCoord;
    float bottomRightXCoord{ bottomRightCoord.x };
    
    //bottomLeftCoord calculation
    glm::vec4 bottomLeftCoord(vertices[8 * 2], vertices[(8 * 2) + 1], vertices[(8 * 2) + 2], 1.0f);
    bottomLeftCoord = projection * view * model * bottomLeftCoord;
    float bottomLeftXCoord{ bottomLeftCoord.x };
    

    float absoluteValueLeftXCoord{ bottomLeftXCoord };
    if (absoluteValueLeftXCoord < 0)
        absoluteValueLeftXCoord = -absoluteValueLeftXCoord;
    
    if (bottomRightXCoord < 0)
        bottomRightXCoord= - bottomRightXCoord;


    float rightXLeftXDist{ bottomRightXCoord + absoluteValueLeftXCoord };

    float fps{ 60 };
    float velocity{ 0.5 };
    float transPerFrame{ (2/fps)*velocity}; //it takes 1 sec to go through whole screen width with a velocity of 1
    float offset{};
    bool isItFirstLoop{ true };

    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
       
        if (isItFirstLoop)
        {
            processInput(window.windowPtr);

            int currentSec{ 0 };
            int count{ 1 };

            while (bottomLeftXCoord <= 1)
            {
                processInput(window.windowPtr);

                int currentFrame{ 1 };
                glfwSetTime(0);
                double t1{ glfwGetTime() };
                std::cerr << 1 / fps;
                while (currentFrame <= fps)
                {
                    processInput(window.windowPtr);
                    if (glfwGetTime() >= t1 + 1 / fps)
                    {
                        //localModel = glm::scale(localModel, glm::vec3(1.0f, 1.0f, 2.0f));


                        //translation matrix
                        localModel = glm::translate(localModel, glm::vec3(transPerFrame, 0.0f, 0.0f));
                        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "localModel"), 1, GL_FALSE, glm::value_ptr(localModel));

                        //background color
                        glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        shader.use();

                        //activate program object
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, grassTex);

                        glBindVertexArray(VAO);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                        glfwSwapBuffers(window.windowPtr);
                        glfwPollEvents();
                        
                        bottomLeftXCoord = bottomLeftXCoord + transPerFrame;             
                        ++currentFrame;
                        std::cout << count << '\n';
                        ++count;
                        t1 = glfwGetTime();
                    }
                }
                ++currentSec;
            }
            isItFirstLoop = false;
            std::cout << "Took " << currentSec << " seconds\n";
            offset = bottomLeftXCoord + rightXLeftXDist ;
        }

        else
        {
            localModel = glm::translate(localModel, glm::vec3((- 1 - offset), 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "localModel"), 1, GL_FALSE, glm::value_ptr(localModel));
           
            bottomLeftXCoord = -1 - rightXLeftXDist;
            int count{ 1 };

            std::cout << ( - 1 - offset ) << "\n" << bottomLeftXCoord << "\n\n";

            while (bottomLeftXCoord <= 1)
            {
                processInput(window.windowPtr);

                int currentFrame{ 1 };
                glfwSetTime(0);
                double t1{ glfwGetTime() };
                while (currentFrame <= fps)
                {
                    processInput(window.windowPtr);
                    if (glfwGetTime() >= t1 + 1 / fps)
                    {
                   
                        //translation matrix
                        localModel = glm::translate(localModel, glm::vec3(transPerFrame, 0.0f, 0.0f));
                        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "localModel"), 1, GL_FALSE, glm::value_ptr(localModel));

                        //background color
                        glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        shader.use();

                        //activate program object 
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, grassTex);

                        glBindVertexArray(VAO);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                        glfwSwapBuffers(window.windowPtr);
                        glfwPollEvents();

                        bottomLeftXCoord = bottomLeftXCoord + transPerFrame;
                        ++currentFrame;
                        //std::cout << count << '\n';
                        ++count;
                        t1 = glfwGetTime();
                    }
                }
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