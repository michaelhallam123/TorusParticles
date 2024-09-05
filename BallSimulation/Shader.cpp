#include "Shader.h"
#include "GL/glew.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    : m_VertexShaderFilePath(vertexShaderFilePath), m_FragmentShaderFilePath(fragmentShaderFilePath), m_ShaderID(0)
{
    std::string vertexShader = ParseShader(vertexShaderFilePath);
    std::string fragmentShader = ParseShader(fragmentShaderFilePath);

    m_ShaderID = CreateShader(vertexShader, fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_ShaderID);
}

std::string Shader::ParseShader(const std::string& path) {

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


unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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


unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_ShaderID);
}
void Shader::Unbind() const
{
    glUseProgram(0);
}

//Set uniforms
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}


void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_ShaderID, name.c_str());

    if (location == -1)
    {
        std::cout << "Warning: Uniform '" << name << "' does not exist!" << std::endl;
    }

    m_UniformLocationCache[name] = location;

    return location;
}