/*
 ********* TORUS PARTICLE SIMULATOR *********
 *
 * A simple program to simulate particles colliding on a torus
 * 
 * Support is available for particles of different masses and radii
 * 
 * 
 */

#include <iostream>

#include "balltype.hpp"
#include "Renderer.hpp"
#include "Solver.hpp"
#include "BruteForceSolver.hpp"
#include "PruneAndSweep1DSolver.hpp"
#include "loadPreset.hpp"

int main()
{
	// Set simulation parameters
	float dt = 0.002f;
    std::vector<balltype> preset = loadPreset("../res/preset1.json");

	// Initialise simulation
    PruneAndSweep1DSolver solver(preset);

    // Initialise renderer
    unsigned int resolution = 860;
    Renderer renderer(solver, resolution);

    float lastTime = 0.0f;

	// Run simulation
    while (renderer.windowOpen())
    {
        float time = glfwGetTime();
        float timeStep = time - lastTime;
        lastTime = time;

        // Draw simulation to window
        renderer.draw();

        // Update physics simulation
        solver.update(dt);
    }

    return 0;

}