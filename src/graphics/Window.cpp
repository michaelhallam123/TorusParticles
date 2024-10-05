#include "Window.hpp"

#include <iostream>


static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

Window::Window(unsigned int xResolution, unsigned int yResolution)
    : m_WindowID(nullptr)
{
    // Set up GLFW window context
    if (!glfwInit())
    {
        std::cout << "Error: failed to initialise GLFW!" << std::endl;
        return;
    }

    m_WindowID = glfwCreateWindow(xResolution, yResolution, "TorusParticles", NULL, NULL);

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

    glViewport(0, 0, std::min(xResolution, yResolution), std::min(xResolution, yResolution));

    glfwSetFramebufferSizeCallback(m_WindowID, framebufferSizeCallback);  

}

Window::~Window()
{
    glfwTerminate();
}

bool Window::isOpen()
{
    return !glfwWindowShouldClose(m_WindowID);
}

void Window::update()
{

}


static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, std::min(width, height), std::min(width, height));
}  