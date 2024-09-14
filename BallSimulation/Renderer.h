#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Solver.hpp"
#include "GLVertex.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer(const Solver& solver);
	~Renderer();

	void Draw(GLFWwindow* window);

private:
	// References to ball data in Solver object
	const std::vector<balltype>& m_ballTypes;
	const std::vector<ball>& m_balls;

	// Vectors of IDs for OpenGL objects
	std::vector<unsigned int> m_vao;
	std::vector<unsigned int> m_vbo;
	std::vector<unsigned int> m_ebo;
	Shader m_shader;

	// Vertex and index data for buffers
	std::vector<GLVertex*> m_vertexData;
	std::vector<unsigned int*> m_indices;
};