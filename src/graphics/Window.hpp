#pragma once

/**
 * Simple wrapper for creating and maintaining GLFW window 
 * object.
 * 
 * Also initialises GLEW on construction.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* m_WindowID;
public:
	Window(unsigned int resolution);
	~Window();

	bool        isOpen();                      // Check window hasn't been closed
	GLFWwindow* getID() { return m_WindowID; } // Get GLFW window ID
};