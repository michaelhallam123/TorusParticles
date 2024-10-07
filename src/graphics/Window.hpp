#pragma once

/**
 * Simple wrapper for creating and maintaining GLFW window 
 * object.
 * 
 * Also initialises GLEW on construction.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Solver.hpp"

class Window
{
private:
	GLFWwindow* m_window;
	const World& m_world;

	// Screen resizing
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
public:
	Window(const Solver& solver, unsigned int xResolution, unsigned int yResolution);
	~Window();

	bool        isOpen();                      // Check window hasn't been closed
	void update();
};