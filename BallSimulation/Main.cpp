/*
 ********* TORUS PARTICLE SIMULATOR *********
 *
 * This program simulates particles colliding on a torus
 * 
 * Support is available for particles of different masses and radii
 * 
 * 
 */



#include <iostream>

#include "vec2.hpp"
#include "balltype.hpp"
#include "Renderer.hpp"
#include "Solver.hpp"
#include "BruteForceSolver.hpp"
#include "PruneAndSweep1DSolver.hpp"

int main()
{
	// Set simulation parameters

	float dt = 0.002f;

    balltype b1;
    b1.radius = 0.15f;
    b1.mass = 2.4f;
    b1.count = 1;
    b1.rgba = { 1.0f, 0.5f, 0.2f, 1.0f };
    b1.totalMomentum = { 0.0f, 0.0f };
    b1.wrapTexture = true;
    b1.render = true;

    balltype b2;
    b2.radius = 0.04f;
    b2.mass = 0.9f;
    b2.count = 40;
    b2.rgba = { 0.0f, 0.5f, 0.8f, 1.0f };
    b2.totalMomentum = { 0.0f, 0.0f };
    b2.wrapTexture = true;
    b2.render = true;

    balltype b3;
    b3.radius = 0.005f;
    b3.mass = 0.1f;
    b3.count = 4000;
    b3.rgba = { 0.7f, 0.1f, 0.6f, 1.0f };
    b3.totalMomentum = { 0.0f, 0.0f };
    b3.wrapTexture = false;
    b3.render = true;

	// Initialise simulation
    PruneAndSweep1DSolver solver({b1, b2, b3});

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