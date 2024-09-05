#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* m_WindowID;
public:
	Window(int resolution);
	~Window();

	bool isOpen();

	void update();
};