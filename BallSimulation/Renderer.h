#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Solver.hpp"
#include "GLVertex.h"
#include "Shader.h"
#include "Window.h"
#include "World.hpp"

class Renderer
{
public:
	Renderer(const Solver& solver, unsigned int resolution);

	void Draw();

	bool windowOpen() { return m_window.isOpen(); }

private:
	// Window object
	Window m_window;

	// References to ball and world data in Solver object
	const std::vector<balltype>& m_ballTypes;
	const std::vector<ball>& m_balls;
	const World& m_world;

	// Vectors of IDs for OpenGL objects
	std::vector<unsigned int> m_vao;
	std::vector<unsigned int> m_vbo;
	std::vector<unsigned int> m_ebo;
	Shader m_shader;

	// Vertex and index data for buffers
	std::vector<std::vector<GLVertex>> m_vertexData;
	std::vector<std::vector<unsigned int>> m_indices;

	
};