#pragma once

#include "Simulation.h"
#include "GLVertex.h"


class Renderer
{
private:
	const Simulation& m_simulation;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ebo;
	GLVertex* m_vertexData;
	unsigned int* m_indices;
public:
	Renderer(const Simulation& simulation);
	~Renderer();

	void Draw();
};

//Renderer should construct and maintain data from:
	//vao
	//vbo
	//ebo
	//shaders