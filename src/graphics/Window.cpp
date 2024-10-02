#include "Window.hpp"

#include <iostream>

Window::Window(unsigned int resolution)
    : m_WindowID(0)
{
    // Set up GLFW window context

    if (!glfwInit())
    {
        std::cout << "Error: failed to initialise GLFW!" << std::endl;
        return;
    }

    m_WindowID = glfwCreateWindow(resolution, resolution, "TorusParticles", NULL, NULL);

    if (!m_WindowID)
    {
        std::cout << "Error: failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_WindowID);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }   

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

Window::~Window()
{
    glfwTerminate();
}

bool Window::isOpen()
{
    return !glfwWindowShouldClose(m_WindowID);
}