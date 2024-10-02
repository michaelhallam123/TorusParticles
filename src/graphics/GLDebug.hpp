#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define ASSERT(x) if (!(x)) __debugbreak();
//#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__));
#define GLCall(x) x

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (GL error code " << error << ") in function " << function <<
            " " << file << ": at line " << line << std::endl;
        return false;
    }
    return true;
}