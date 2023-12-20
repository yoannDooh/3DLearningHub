#include "../header/openGL.h"


/*--SHADER CLASSE--*/
static std::string readGlslFile(std::string filePath)
{
    std::string shader{};
    std::string offStream{};
    std::ifstream shaderFile(filePath);

    while (std::getline(shaderFile, offStream))
    {
        if (offStream != "")
        {
            shader = shader + '\n' + offStream;
        }
    }

    return shader;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexShaderCppStr{ readGlslFile(vertexPath) };
    const char* vertexShaderSrc{ vertexShaderCppStr.c_str() };

    std::string fragmentShaderCppStr{ readGlslFile(fragmentPath) };
    const char* fragmentShaderSrc{ fragmentShaderCppStr.c_str() };
    
    //fragment shader 
    unsigned int fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);

    //vertex shader
    unsigned int vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);


    //debug shader and fragment
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;

        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    //link shaders
    Shader::ID = { glCreateProgram() };
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    //debug shader program
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    //delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set4Float(const std::string& name, float values[4]) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), values[0], values[1], values[2], values[3]);
}


/*--WINDOW CLASSE--*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Window::Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle)
{
    // glfw: initialize and configure
      // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "LearnOpenGL", NULL, NULL);
    windowPtr = window;

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        didWindowFailed = -1;
    }
    glfwMakeContextCurrent(windowPtr);
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        didWindowFailed = -1;
    }
}


/*--SHADER CLASSE--*/
Texture::Texture(int textureNb, std::string textureName,  const char* texturePath)
{

    int width{}, height{}, nrChannels{};

    unsigned int texture{};
    Texture::textureNb = textureNb;
    glGenTextures(1, &texture);
    Texture::ID = texture;

    Texture::data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
   

    if (data)
    {
        GLenum format = GL_RGBA;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    else
       std::cout << "Failed to load : " << textureName << std::endl;
}

void Texture::activate()
{
    switch (Texture::textureNb)
    {
    case 0 :
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture::ID);
        break; 

    case 1:
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture::ID);
        break;

    case 2:
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Texture::ID);
        break;

    case 3:
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Texture::ID);
        break;

    }
}