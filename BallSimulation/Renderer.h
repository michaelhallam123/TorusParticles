#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Solver.hpp"
#include "State.hpp"
#include "GLVertex.h"
#include "Shader.h"

class Renderer
{
private:
	const State& m_state;
	std::vector<unsigned int> m_vao;
	std::vector<unsigned int> m_vbo;
	std::vector<unsigned int> m_ebo;
	std::vector<GLVertex*> m_vertexData;
	std::vector<unsigned int*> m_indices;
	std::vector<balltype> m_ballTypes;
	Shader m_shader;
public:
	Renderer(const Solver& solver);
	~Renderer();

	void Draw(GLFWwindow* window);
};