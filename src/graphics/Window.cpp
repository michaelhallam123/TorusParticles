#include "Window.hpp"

#include <iostream>

Window::Window(const Solver& solver, unsigned int xResolution, unsigned int yResolution)
    : m_windowID(nullptr),
      m_world(solver.getWorld())
{
    // Set up GLFW window context
    if (!glfwInit())
    {
        std::cout << "Error: failed to initialise GLFW!" << std::endl;
        return;
    }

    m_windowID = glfwCreateWindow(xResolution, yResolution, "TorusParticles", NULL, NULL);

    if (!m_windowID)
    {
        std::cout << "Error: failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_windowID);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set glfw window user pointer for callbacks
    glfwSetWindowUserPointer(m_windowID, this);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }   

    // Configure screen resizing
    framebufferSizeCallback(m_windowID, xResolution, yResolution);

    glfwSetFramebufferSizeCallback(
        m_windowID, 
        [](GLFWwindow* window, int width, int height)
        { 
            Window* myWindow = (Window*)glfwGetWindowUserPointer(window);
            myWindow->framebufferSizeCallback(window, width, height);
        }
    );  
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::isOpen()
{
    return !glfwWindowShouldClose(m_windowID);
}

void Window::update()
{
    glfwSwapBuffers(m_windowID);

    glfwPollEvents();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    float worldAspectRatio = m_world.xWidth / m_world.yWidth;

    if ((float)width/(float(height)) <= worldAspectRatio)
    {
        int yLower = static_cast<int>((float)height / 2.0f - (float)width / (2.0f*worldAspectRatio));
        int adjustedHeight = static_cast<int>((float)width / worldAspectRatio);

        glViewport(0, yLower, width, adjustedHeight);
    }
    else
    {
        int xLower  = static_cast<int>((float)width / 2.0f - ((float)height*worldAspectRatio) / 2.0f);
        int adjustedWidth = static_cast<int>((float)height*worldAspectRatio);

        glViewport(xLower, 0, adjustedWidth, height);
    }
}  