#include "../header/shader.h"
#include "../header/motion.h"
#include <iostream>
#include <fstream>

#define UNIFORM_BUFFER_NB 3
#define MAX_LIGHTS_NB 200
#define DIRECT_LIGHTS_NB 1

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

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    std::string vertexShaderCppStr{ readGlslFile(vertexPath) };
    const char* vertexShaderSrc{ vertexShaderCppStr.c_str() };

    std::string fragmentShaderCppStr{ readGlslFile(fragmentPath) };
    const char* fragmentShaderSrc{ fragmentShaderCppStr.c_str() };

    std::string geometryShaderCppStr{ readGlslFile(geometryPath) };
    const char* geometryShaderSrc{ geometryShaderCppStr.c_str() };

    //vertex shader
    unsigned int vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    //geometry shader
    unsigned int geometryShader { glCreateShader(GL_GEOMETRY_SHADER) };
    glShaderSource(geometryShader, 1, &geometryShaderSrc, NULL);
    glCompileShader(geometryShader);

    //fragment shader 
    unsigned int fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);


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

        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    //link shaders
    Shader::ID = { glCreateProgram() };
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glAttachShader(ID, geometryShader);
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
    glDeleteShader(geometryShader);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessellationControl, const char* tessellationEvaluation)
{
    std::string vertexShaderCppStr{ readGlslFile(vertexPath) };
    const char* vertexShaderSrc{ vertexShaderCppStr.c_str() };

    std::string fragmentShaderCppStr{ readGlslFile(fragmentPath) };
    const char* fragmentShaderSrc{ fragmentShaderCppStr.c_str() };

    std::string tcShaderCppStr{ readGlslFile(tessellationControl) };
    const char* tcShaderSrc{ tcShaderCppStr.c_str() };

    std::string teShaderCppStr{ readGlslFile(tessellationEvaluation) };
    const char* teShaderSrc{ teShaderCppStr.c_str() };

    //vertex shader
    unsigned int vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    //tessellationControl shader
    unsigned int tessellationControlShader{ glCreateShader(GL_TESS_CONTROL_SHADER) };
    glShaderSource(tessellationControlShader, 1, &tcShaderSrc, NULL);
    glCompileShader(tessellationControlShader);

    //tessellationEvaluation shader
    unsigned int tessellationEvaluationShader { glCreateShader(GL_TESS_EVALUATION_SHADER) };
    glShaderSource(tessellationEvaluationShader, 1, &teShaderSrc, NULL);
    glCompileShader(tessellationEvaluationShader);

    //fragment shader 
    unsigned int fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);


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

        glGetShaderInfoLog(tessellationControlShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::TESSELATION_CONTROL::COMPILATION_FAILED\n" <<
            infoLog << std::endl;

        glGetShaderInfoLog(tessellationEvaluationShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::TESSELATION_EVALUATION::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    //link shaders
    Shader::ID = { glCreateProgram() };
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glAttachShader(ID, tessellationControlShader);
    glAttachShader(ID, tessellationEvaluationShader);
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
    glDeleteShader(tessellationControlShader);
    glDeleteShader(tessellationEvaluationShader);
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

void Shader::set2Float(const std::string& name, glm::vec2& value) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);

}

void Shader::set2Float(const std::string& name,  std::array<float,2>values) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), values[0], values[1]);
}

void Shader::set3Float(const std::string& name, float values[3]) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), values[0], values[1], values[2]);
}

void Shader::set3Float(const std::string& name, glm::vec3& value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);

}

void Shader::set4Float(const std::string& name, float values[4]) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), values[0], values[1], values[2], values[3]);

}

void Shader::setMat4(const std::string& name, glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str() ), 1, GL_FALSE, glm::value_ptr(mat));

}

//UBO AND SSO FUNCTIONS / VARIABLES

std::array<unsigned int, UNIFORM_BUFFER_NB> buffersId{};

void genUbo0()
{
    std::size_t size{ 2 * sizeof(glm::mat4) + sizeof(glm::vec4) };

    unsigned int uniformBuff;
    glGenBuffers(1, &uniformBuff);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuff);
    glBufferData(GL_UNIFORM_BUFFER,size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuff);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    buffersId[0] = uniformBuff;
}

void fillUbo0(int dataToFill) 
{
    glm::vec4 cameraPos{};

    glBindBuffer(GL_UNIFORM_BUFFER, buffersId[0] );

    switch (dataToFill)
    {
        case 0 : 
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(World::view));
            break;

        case 1:
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(World::projection));
            break;

        case 2:
            cameraPos = glm::vec4(World::camera.pos, 0.0f);
            glBufferSubData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(cameraPos));
            break;
             
        default : 
            std::cerr << "INCORECT INDEX FOR UBO 0";
            break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void genUbo1()
{
    /* STD140 layout rules
    struct PointLight size/offset :
    {
        vec3 color; // offset 0
        vec3 pos;   // offset 16 (+16)
    
        vec3 ambient;  // offset 32 (+16)
        vec3 diffuse;  // offset 48 (+16)
        vec3 specular;  // offset 64 (+16)
    
        float constant;  // offset 76 (+12)
        float linearCoef; // offset 80 (+4)
        float squareCoef; // offset 84 (+4)
    
        //bottom-padding: +12, to be the next multiple of 16 (size of vec4)
        //total-size : 96 bits
    };
    */

    std::size_t size{ MAX_LIGHTS_NB*96 };

    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    buffersId[1] = ssbo;
}

void fillUbo1(int lightIndex, int dataToFill)
{
    /* STD140 layout rules
    struct PointLight size/offset : 
    {
        vec3 color; // offset 0 
        vec3 pos;   // offset 16 (+16)

        vec3 ambient;  // offset 32 (+16)
        vec3 diffuse;  // offset 48 (+16)
        vec3 specular;  // offset 64 (+16)

        float constant;  // offset 76 (+12)
        float linearCoef; // offset 80 (+4)
        float squareCoef; // offset 84 (+4)

        //bottom-padding: +12, to be the nearest up multiple of 16 (size of vec4)
        //total-size : 96 bits
    };
    */

    int structSize{ 96 }; //refer to std140 layout rule 
    int offset{ lightIndex * structSize }; //refer to std140 layout rule 

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffersId[1]);

    if (dataToFill == -1)
    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].color);

        offset += 16;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].pos);

        offset += 16;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].ambient);

        offset += 16;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].diffuse);

        offset += 16;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].specular);

        offset += 12;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].constant);

        offset += 4;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].linearCoef);

        offset += 4;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].squareCoef);

        return;
    }

    switch (dataToFill)
    {
        case 0:
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].color);
            break;

        case 1:
            offset += 16;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].pos);
            break;

        case 2:
            offset += 16 * 2;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].ambient);
            break;

        case 3:
            offset += 16 * 3;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].diffuse);
            break;

        case 4:
            offset += 16 * 4;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec3), &World::lightPoints[lightIndex].specular);
            break;

        case 5:
            offset += 16 * 4 + 12;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].constant);
            break;
       
       
        case 6:
            offset += 16 * 4 + 12 + 4;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].linearCoef);
            break;


        case 7:
            offset += 16 * 4 + 12 + 4*2;
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float), &World::lightPoints[lightIndex].squareCoef);
            break;
       
        default:
            std::cerr << "UKNOWN INDEX OF DATA TO FILL FOR UBO2" << std::endl;
            break;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void genUbo2()
{
    /* STD140 layout rules
    struct DirectLight { 
    vec3 color;  // offset 0
    vec3 direction; // offset 16 (+16)

    vec3 ambient; // offset 32 (+16) 
    vec3 diffuse; // offset 48 (+16)
    vec3 specular; // offset 64 (+16)
    
    //bottom-padding: +16, to be the next multiple of 16 (size of vec4)
    //total-size : 80 bits
    };
   */

    std::size_t size{ DIRECT_LIGHTS_NB*80 };

    unsigned int uniformBuff;
    glGenBuffers(1, &uniformBuff);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuff);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, uniformBuff);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    buffersId[2] = uniformBuff;
}

void fillUbo2(int lightIndex,int dataToFill) 
{
    /*STD140 layout rules
       struct DirectLight {
       vec3 color;  // offset 0
       vec3 direction; // offset 16 (+16)

       vec3 ambient; // offset 32 (+16)
       vec3 diffuse; // offset 48 (+16)
       vec3 specular; // offset 64 (+16)

       +16 
       //total-size : 64 bits
       };
      */
    
    int structSize{ 64 }; //refer to std140 layout rule 
    int offset{ lightIndex * structSize }; //refer to std140 layout rule 

    glBindBuffer(GL_UNIFORM_BUFFER, buffersId[2]);

    if (dataToFill == -1)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].color);

        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].direction);

        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].ambient);

        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].diffuse);

        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].specular);

        return;
    }

    switch (dataToFill)
    {
    case 0:
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].color);
        break;

    case 1:
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].direction);
        break;

    case 2:
        offset += 16 * 2;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].ambient);
        break;

    case 3:
        offset += 16 * 3;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].diffuse);
        break;

    case 4:
        offset += 16 * 4;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &World::directLights[lightIndex].specular);
        break;

    default:
        std::cerr << "UKNOWN INDEX OF DATA TO FILL FOR UBO2" << std::endl;
        break;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}