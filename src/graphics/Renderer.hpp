#pragma once

/**
 * Renderer class for creating and managing an OpenGL context, 
 * and drawing the simulation to the screen. 
 */

#include <vector>
#include <array>

#include "Preset.hpp"
#include "Solver.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "World.hpp"

class Renderer
{
public:
	Renderer(const Solver& solver, Preset preset, unsigned int xResolution, unsigned int yResolution);

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
	std::vector<unsigned int> m_VAOs;
	std::vector<unsigned int> m_positionVBOs;
	Shader m_shader;

	// Vertex data
	static const unsigned int MAX_QUADS = 9;
	static const unsigned int VERTICES_PER_QUAD = 6;

	// Texture coordinate data (common to all BallTypes)
	std::array<Vec2<float>, MAX_QUADS * VERTICES_PER_QUAD> m_texCoords;
	unsigned int m_texCoordsVBO;

	// Offset data (for rendering 9 copies of each Ball whose BallType has wrapTexture==true)
	std::array<Vec2<float>, MAX_QUADS * VERTICES_PER_QUAD> m_offsets;
	unsigned int m_offsetsVBO;

	// Helper function
	void setVertexAttributes (std::size_t ballTypeIndex);
};