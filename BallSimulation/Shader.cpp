#include "Shader.hpp"
#include "GL/glew.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    : m_vertexShaderFilePath(vertexShaderFilePath), 
      m_fragmentShaderFilePath(fragmentShaderFilePath), 
      m_shaderID(0)
{
    std::string vertexShader = parseShader(vertexShaderFilePath);
    std::string fragmentShader = parseShader(fragmentShaderFilePath);

    m_shaderID = createShader(vertexShader, fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderID);
}

std::string Shader::parseShader(const std::string& path) {

    std::stringstream result;
    std::ifstream file(path);

    if (file.is_open())
    {
        std::string line;

        while (getline(file, line))
        {
            result << line << '\n';
        }
        file.close();
    }
    else
        std::cout << "Unable to open file " << path << std::endl;

    return result.str();

}


unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error checking:
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        char message[512];
        glGetShaderInfoLog(id, 512, NULL, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}


unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::bind() const
{
    glUseProgram(m_shaderID);
}
void Shader::unbind() const
{
    glUseProgram(0);
}

//Set uniforms
void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform4f(const std::string& name, std::array<float, 4> v)
{
    glUniform4f(getUniformLocation(name), v[0], v[1], v[2], v[3]);
}


void Shader::setUniform1i(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

int Shader::getUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
    {
        return m_uniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_shaderID, name.c_str());

    if (location == -1)
    {
        std::cout << "Warning: Uniform '" << name << "' does not exist!" << std::endl;
    }

    m_uniformLocationCache[name] = location;

    return location;
}