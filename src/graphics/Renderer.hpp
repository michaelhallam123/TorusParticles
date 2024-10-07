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
#include <array>

#include "Solver.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "World.hpp"

class Renderer
{
public:
	Renderer(const Solver& solver, unsigned int xResolution, unsigned int yResolution);

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

	// Texture coordinate data (common to all BallTypes)
	std::array<Vec2<float>, 9*6> m_texCoords;
	unsigned int m_texCoordsVBO;

	// Offset data (for rendering 9 copies of each Ball whose BallType has wrapTexture==true)
	std::array<Vec2<float>, 9*6> m_offsets;
	unsigned int m_offsetsVBO;
};