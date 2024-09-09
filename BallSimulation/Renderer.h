#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "GLVertex.h"
#include "Shader.h"

class Renderer
{
private:
	const Simulation& m_simulation;
	std::vector<unsigned int> m_vao;
	std::vector<unsigned int> m_vbo;
	std::vector<unsigned int> m_ebo;
	std::vector<GLVertex*> m_vertexData;
	std::vector<unsigned int*> m_indices;
	Shader m_shader;
public:
	Renderer(const Simulation& simulation);
	~Renderer();

	void Draw(GLFWwindow* window);
};