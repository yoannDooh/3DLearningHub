#include "../header/shader.h"
#include "../header/window.h"

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float vertices[]{
                        //coordinates           //colors
                        0.5f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f,       // top right
                        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,        // bottom right
                        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,       // bottom left
                        -0.5f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,       // top left
                    };

unsigned int indices[]  { 
0, 1, 3, // first triangle
1, 2, 3// second triangle
};

int main()
{  
    Window window(SCR_WIDTH, SCR_HEIGHT, "learnOpengl");

    const char* vertexPath{ "C:\\Users\\yoann\\OneDrive\\Bureau\\Proggramation\\apprendre\\openGL\\exercices\\test\\src\\vertexShader.glsl" };
    const char* fragmentPath{"C:\\Users\\yoann\\OneDrive\\Bureau\\Proggramation\\apprendre\\openGL\\exercices\\test\\src\\fragmentShader.glsl"};
    Shader shader(vertexPath, fragmentPath);

    // initialize VAO
    unsigned int VAO{};
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    
    // initialize EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    

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

    // render loop
    while (!glfwWindowShouldClose(window.windowPtr))
    {
        processInput(window.windowPtr);

        glClearColor(1.0f, 72.0f/255.0f, 95.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
  
        shader.use();

        //activate program object 
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,(void *)0 );
        glBindVertexArray(0);
        
        
        //glDrawArrays(GL_TRIANGLES, 0, 3);
     
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
