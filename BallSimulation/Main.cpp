//TO DO:
    // Fix antialiasing so that balls actually touch on collision - may need to alter texture coordinates for this
    // Also antialiasing still looks somewhat jagged
    // Allow for screen resizing
    // Separate physics from framerate (!)
    // Add gui for easier control of parameters
    // Speed up simulation using multithreading
    // Add pause and step through simulation functionality
    // Add cute window icon
    // Make the main file cleaner, maybe just give simulation a run method containing the while loop

#include <iostream>

#include "vec2.h"
#include "balltype.hpp"
#include "Renderer.h"
#include "Solver.hpp"
#include "BruteForceSolver.hpp"
#include "PruneAndSweep1DSolver.hpp"

int main()
{
	// Set simulation parameters

	float dt = 0.002f;

    balltype b1;
    b1.radius = 0.15f;
    b1.mass = 4.4f;
    b1.count = 1;
    b1.rgba = { 1.0f, 0.5f, 0.2f, 1.0f };
    b1.totalMomentum = { 0.0f, 0.0f };
    b1.wrapTexture = true;

    balltype b2;
    b2.radius = 0.04f;
    b2.mass = 0.9f;
    b2.count = 40;
    b2.rgba = { 0.0f, 0.5f, 0.8f, 1.0f };
    b2.totalMomentum = { 0.0f, 0.0f };
    b2.wrapTexture = true;

    balltype b3;
    b3.radius = 0.005f;
    b3.mass = 0.1f;
    b3.count = 7000;
    b3.rgba = { 0.0f, 0.0f, 0.0f, 1.0f };
    b3.totalMomentum = { -200.0f, 0.0f };
    b3.wrapTexture = false;

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
        renderer.Draw();

        // Update physics simulation
        solver.update(dt);
    }

    return 0;

}