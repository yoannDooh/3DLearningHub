#include "../header/stb_image.h"
#include "../header/openGL.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float vertices[]{ 
    //face 1  (in order: GCBH): 
            //coordinates        //colors               //texture coord
            0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 1.0f,      1.0f, 1.0f,       // top right
            0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,      1.0f, 0.0f,       // bottom right
            -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f,      0.0f, 0.0f,        // bottom left
            -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 1.0f,      0.0f, 1.0f,        // top left

    //face 2 (FDAE) :
            0.5f, 0.5f,  -0.5f,   0.0f, 1.0f, 0.0f,     1.0f, 1.0f,       // top right
            0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,      1.0f, 0.0f,       // bottom right
            -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 0.0f,       0.0f, 0.0f,        // bottom left
            -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 0.0f,      0.0f, 1.0f,        // top left

    //face 3 (HBAE) :
             -0.5f, 0.5f, 0.5f,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f,       // top right
             -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 1.0f,      1.0f, 0.0f,       // bottom right
             -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 0.0f,      0.0f, 0.0f,        // bottom left
             -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 0.0f,     0.0f, 1.0f,        // top left

    //face 4 (GCDF) :
             0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 1.0f,      1.0f, 1.0f,      // top right
             0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             0.5f, -0.5f,-0.5f,  0.0f, 0.0f, 0.0f,      0.0f, 0.0f,       // bottom left
             0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,     0.0f, 1.0f,        // top left

    //face 5 (FGHE) :
             0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,      1.0f, 1.0f,      // top right
             0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 1.0f,      0.0f, 0.0f,       // bottom left
             -0.5f, 0.5f, -0.5f,  1.0f, 1.0f, 0.0f,     0.0f, 1.0f,        // top left

     //face 6 (DCBA) :
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,      1.0f, 1.0f,      // top right
             0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,      1.0f, 0.0f,      // bottom right
             -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 1.0f,      0.0f, 0.0f,       // bottom left
             -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,     0.0f, 1.0f,        // top left
};

unsigned int indices[]{
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

int main()
{
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");
    const char* vertexPath{ ".\\src\\vertexShader.glsl" };
    const char* fragmentPath{ ".\\src\\fragmentShader.glsl" };
    Shader shader(vertexPath, fragmentPath);
    glEnable(GL_DEPTH_TEST);


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

    //local
    glm::mat4 localModel{ glm::mat4(1.0f) };
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "localModel"), 1, GL_FALSE, glm::value_ptr(localModel));

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


    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        //background color
        glClearColor(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);
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
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}