#pragma once

/**
 * Renderer class for creating and managing an OpenGL context, 
 * and drawing the simulation to the screen. 
 * 
 * Stores const references to the simulation data at
 * instantiation, so must be constructed after the Solver 
 * object.
 */

#include <vector>

#include "Solver.hpp"
#include "GLVertex.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "World.hpp"

class Renderer
{
public:
	Renderer(const Solver& solver, unsigned int resolution);

	void draw();                                    // Draw simulation to window
	bool windowOpen() { return m_window.isOpen(); } // Check window is still open

private:
	// Window object
	Window m_window;

	// References to ball and world data in Solver object
	const std::vector<BallType>& m_ballTypes;
	const std::vector<Ball>&     m_balls;
	const World&                 m_world;

	// Vectors of IDs for OpenGL objects
	std::vector<unsigned int> m_vaos;
	std::vector<unsigned int> m_vbos;
	std::vector<unsigned int> m_ebos;
	Shader m_shader;

	// Vectors of vertex and index data for OpenGL buffers
	std::vector<std::vector<GLVertex>>    m_vertexData;
	std::vector<std::vector<std::size_t>> m_indices;

	void updateVertexData();
};