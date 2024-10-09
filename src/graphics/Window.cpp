#include "Window.hpp"

#include <iostream>

Window::Window(const Solver& solver, unsigned int xResolution, unsigned int yResolution)
    : m_window(nullptr),
      m_world(solver.getWorld())
{
    // Set up GLFW window context
    if (!glfwInit())
    {
        std::cout << "Error: failed to initialise GLFW!" << std::endl;
        std::cin.get();
        return;
    }

    m_window = glfwCreateWindow(xResolution, yResolution, "TorusParticles", NULL, NULL);

    if (!m_window)
    {
        std::cout << "Error: failed to create GLFW window!" << std::endl;
        std::cin.get();
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Set glfw window user pointer for callbacks
    glfwSetWindowUserPointer(m_window, this);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }   

    // Configure screen resizing
    framebufferSizeCallback(m_window, xResolution, yResolution);

    glfwSetFramebufferSizeCallback(
        m_window, 
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
    return !glfwWindowShouldClose(m_window);
}

void Window::update()
{
    glfwSwapBuffers(m_window);

    glfwPollEvents();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
/**
 * Screen resizing callback. Maintains the world's
 * aspect ratio while keeping the location on the
 * screen central.
 */
{
    float worldAspectRatio = m_world.xWidth / m_world.yWidth;
    float screenAspectRatio = static_cast<float>(width)/height;

    if (screenAspectRatio <= worldAspectRatio)
    {
        int yLower = static_cast<int>(
            0.5f * (static_cast<float>(height) - static_cast<float>(width) / worldAspectRatio)
        );

        int adjustedHeight = static_cast<int>(
            static_cast<float>(width) / worldAspectRatio
        );

        glViewport(0, yLower, width, adjustedHeight);
    }
    else
    {
        int xLower  = static_cast<int>(
            0.5f * (static_cast<float>(width) - static_cast<float>(height) * worldAspectRatio)
        );

        int adjustedWidth = static_cast<int>(
            static_cast<float>(height) * worldAspectRatio
        );

        glViewport(xLower, 0, adjustedWidth, height);
    }
}  