//TO DO:
    // Fix antialiasing so that balls actually touch on collision - may need to alter texture coordinates for this
    // Also antialiasing still looks somewhat jagged
    // Allow for screen resizing
    // Separate physics from framerate (!)
    // Make circles loop smoothly from one side of the screen to the opposite (probably much easier using shader)
    // Add gui for easier control of parameters
    // Speed up simulation using multithreading
    // Add pause and step through simulation functionality
    // Add cute window icon
    // Make the main file cleaner, maybe just give simulation a run method containing the while loop

#include <iostream>

#include "vec2.h"
#include "balltype.hpp"
#include "Renderer.h"
#include "Window.h"
#include "Solver.hpp"
#include "BruteForceSolver.hpp"
#include "PruneAndSweep1DSolver.hpp"
#include "PruneAndSweep2DSolver.hpp"

int main()
{
    // Create window

    int resolution = 860;
    Window window(resolution);

	// Set simulation parameters

	float dt = 0.002f;

    balltype b1;
    b1.radius = 0.15f;
    b1.mass = 4.4f;
    b1.count = 1;
    b1.rgba = { 1.0f, 0.5f, 0.2f, 1.0f };
    b1.totalMomentum = { -100.0f, 0.0f };

    balltype b2;
    b2.radius = 0.04f;
    b2.mass = 0.9f;
    b2.count = 40;
    b2.rgba = { 0.0f, 0.5f, 0.8f, 1.0f };
    b2.totalMomentum = { 0.0f, 0.0f };

    balltype b3;
    b3.radius = 0.005f;
    b3.mass = 0.1f;
    b3.count = 5000;
    b3.rgba = { 1.0f, 0.5f, 0.8f, 1.0f };
    b3.totalMomentum = { 0.0f, 0.0f };

	// Initialise simulation and renderer
    PruneAndSweep1DSolver solver({b1, b3});
    Renderer renderer(solver);

    float lastTime = 0.0f;

	// Run simulation
    while (window.isOpen())
    {
        float time = glfwGetTime();
        float timeStep = time - lastTime;
        lastTime = time;

        // Draw simulation to window
        renderer.Draw(window.getID());

        // Update window events, swap buffers
        window.update();

        // Update physics simulation
        solver.update(dt);
    }

    return 0;

}